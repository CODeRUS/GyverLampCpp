#include "SoundStereoEffect.h"

#include <math.h>
#if defined(ESP32)
#include <driver/adc.h>
#endif
#include <arduinoFFT.h>

namespace  {

#if defined(ESP32)
adc1_channel_t leftChannel = ADC1_CHANNEL_6;
adc1_channel_t rightChannel = ADC1_CHANNEL_7;
#endif

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

unsigned long newTime;

struct eqBand {
    uint16_t amplitude;
    byte bandWidth;
    int peak;
    int lastpeak;
    uint16_t curval;
    uint16_t lastval;
    unsigned long lastmeasured;
};

eqBand laudiospectrum[EQBANDS] = {
    /*
                 Adjust the amplitude/bandWidth values
                 to fit your microphone
              */
    { 1000, 2,   0, 0, 0, 0, 0}, // 125
    { 500,  2,   0, 0, 0, 0, 0}, // 250
    { 300,  3,   0, 0, 0, 0, 0}, // 500
    { 250,  7,   0, 0, 0, 0, 0}, // 1k
    { 100,  14,  0, 0, 0, 0, 0}, // 2k
    { 100,  24,  0, 0, 0, 0, 0}, // 4k
    { 100,  48,  0, 0, 0, 0, 0}, // 8k
    { 100,  155, 0, 0, 0, 0, 0}  // 16k
};

eqBand raudiospectrum[EQBANDS] = {
    /*
                 Adjust the amplitude/bandWidth values
                 to fit your microphone
              */
    { 1000, 2,   0, 0, 0, 0, 0}, // 125
    { 500,  2,   0, 0, 0, 0, 0}, // 250
    { 300,  3,   0, 0, 0, 0, 0}, // 500
    { 250,  7,   0, 0, 0, 0, 0}, // 1k
    { 100,  14,  0, 0, 0, 0, 0}, // 2k
    { 100,  24,  0, 0, 0, 0, 0}, // 4k
    { 100,  48,  0, 0, 0, 0, 0}, // 8k
    { 100,  155, 0, 0, 0, 0, 0}  // 16k
};

/* store bandwidth variations when sample rate changes */
int lbandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};
int rbandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

} // namespace

SoundStereoEffect::SoundStereoEffect(const String &id)
    : Effect(id)
{
#if defined(ESP32)
    adc1_config_width(ADC_WIDTH_BIT_12);   //Range 0-1023
    adc1_config_channel_atten(leftChannel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
    adc1_config_channel_atten(rightChannel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
#endif
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    delay(1000);
    setBandwidth();
}

void SoundStereoEffect::tick()
{
    double* lvReal = new double[SAMPLES]();
    double* lvImag = new double[SAMPLES]();
    double* rvReal = new double[SAMPLES]();
    double* rvImag = new double[SAMPLES]();

    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
#if defined(ESP32)
        lvReal[i] = adc1_get_raw(leftChannel); // A raw conversion takes about 20uS on an ESP32
        rvReal[i] = adc1_get_raw(rightChannel); // A raw conversion takes about 20uS on an ESP32
        delayMicroseconds(20);
#else
        lvReal[i] = rvReal[i] = analogRead(A0); // A conversion takes about 1uS on an ESP32
#endif

        lvImag[i] = 0;
        rvImag[i] = 0;

        while ((micros() - newTime) < sampling_period_us) {
            // do nothing to wait
            yield();
        }
    }

    arduinoFFT FFT1(lvReal, lvImag, SAMPLES, samplingFrequency);
    FFT1.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT1.Compute(FFT_FORWARD);
    FFT1.ComplexToMagnitude();

    delete[] lvImag;

    arduinoFFT FFT2(rvReal, rvImag, SAMPLES, samplingFrequency);
    FFT2.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT2.Compute(FFT_FORWARD);
    FFT2.ComplexToMagnitude();

    delete[] rvImag;

    myMatrix->clear();

    int readLBands[EQBANDS] = {0};
    int readRBands[EQBANDS] = {0};
    for (int i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (lvReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getLBand(i);
            int read = (int)lvReal[i];
            if (bandNum != bands && readLBands[bandNum] < read) {
                readLBands[bandNum] = read;
            }
        }
        if (rvReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getRBand(i);
            int read = (int)rvReal[i];
            if (bandNum != bands && readRBands[bandNum] < read) {
                readRBands[bandNum] = read;
            }
        }
    }

    delete[] lvReal;
    delete[] rvReal;

    for (int bandNum = 0; bandNum < EQBANDS; bandNum++) {
        if (readLBands[bandNum] > 0 && laudiospectrum[bandNum].curval != readLBands[bandNum]) {
            laudiospectrum[bandNum].curval = readLBands[bandNum];
        }
        if (readRBands[bandNum] > 0 && raudiospectrum[bandNum].curval != readRBands[bandNum]) {
            raudiospectrum[bandNum].curval = readRBands[bandNum];
        }
        displayLBand(bandNum);
        displayRBand(bandNum);
    }

}

void SoundStereoEffect::displayLBand(int band)
{
    int dsize = laudiospectrum[band].curval;
    int dmax = mySettings->matrixSettings.height;
//    int ssize = dsize;
    int fsize = dsize / laudiospectrum[band].amplitude;
    double factor = settings.scale / 100.0;
    dsize = fsize * factor;
//    Serial.printf("displayLBand %d, %d, %d, %f, %d\n", band, ssize, fsize, factor, dsize);
    if (dsize > dmax) {
        dsize = dmax;
    }
    for (int y = 0; y < dsize; y++) {
        myMatrix->drawPixelXY(band * 2, y, CRGB(CRGB::Blue));
    }
    if (dsize > laudiospectrum[band].peak) {
        laudiospectrum[band].peak = dsize;
    }
    laudiospectrum[band].lastval = dsize;
    laudiospectrum[band].lastmeasured = millis();
}

void SoundStereoEffect::displayRBand(int band)
{
    int dsize = raudiospectrum[band].curval;
    int dmax = mySettings->matrixSettings.height - 1;
//    int ssize = dsize;
    int fsize = dsize / raudiospectrum[band].amplitude;
    double factor = settings.scale / 100.0;
    dsize = fsize * factor;
//    Serial.printf("displayRBand %d, %d, %d, %f, %d\n", band, ssize, fsize, factor, dsize);
    if (dsize > dmax) {
        dsize = dmax;
    }
    for (int y = 0; y < dsize; y++) {
        myMatrix->drawPixelXY(band * 2 + 1, y, CRGB(CRGB::Green));
    }
    if (dsize > raudiospectrum[band].peak) {
        raudiospectrum[band].peak = dsize;
    }
    raudiospectrum[band].lastval = dsize;
    raudiospectrum[band].lastmeasured = millis();
}

void SoundStereoEffect::setBandwidth()
{
    byte multiplier = SAMPLES / 256;
    lbandWidth[0] = laudiospectrum[0].bandWidth * multiplier;
    for (byte j = 1; j < EQBANDS; j++) {
        lbandWidth[j] = laudiospectrum[j].bandWidth * multiplier + lbandWidth[j - 1];
    }
    rbandWidth[0] = raudiospectrum[0].bandWidth * multiplier;
    for (byte j = 1; j < EQBANDS; j++) {
        rbandWidth[j] = raudiospectrum[j].bandWidth * multiplier + rbandWidth[j - 1];
    }
}

byte SoundStereoEffect::getLBand(int i)
{
    for (byte j = 0; j < EQBANDS; j++) {
        if (i <= lbandWidth[j]) {
            return j;
        }
    }
    return EQBANDS;
}

byte SoundStereoEffect::getRBand(int i)
{
    for (byte j = 0; j < EQBANDS; j++) {
        if (i <= rbandWidth[j]) {
            return j;
        }
    }
    return EQBANDS;
}
