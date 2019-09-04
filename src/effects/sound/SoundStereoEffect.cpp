#include "SoundStereoEffect.h"

#include <math.h>
#include <driver/adc.h>
#include <arduinoFFT.h>

namespace  {

arduinoFFT FFT = arduinoFFT();

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

double lvReal[SAMPLES];
double lvImag[SAMPLES];
double rvReal[SAMPLES];
double rvImag[SAMPLES];
unsigned long newTime;

bool adcread = true;

struct eqBand {
    const char *freqname;
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
    { "125Hz", 1000, 2,   0, 0, 0, 0, 0},
    { "250Hz", 500,  2,   0, 0, 0, 0, 0},
    { "500Hz", 300,  3,   0, 0, 0, 0, 0},
    { "1KHz",  250,  7,   0, 0, 0, 0, 0},
    { "2KHz",  100,  14,  0, 0, 0, 0, 0},
    { "4KHz",  100,  24,  0, 0, 0, 0, 0},
    { "8KHz",  100,  48,  0, 0, 0, 0, 0},
    { "16KHz", 100,  155, 0, 0, 0, 0, 0}
};

eqBand raudiospectrum[EQBANDS] = {
    /*
                 Adjust the amplitude/bandWidth values
                 to fit your microphone
              */
    { "125Hz", 1000, 2,   0, 0, 0, 0, 0},
    { "250Hz", 500,  2,   0, 0, 0, 0, 0},
    { "500Hz", 300,  3,   0, 0, 0, 0, 0},
    { "1KHz",  250,  7,   0, 0, 0, 0, 0},
    { "2KHz",  100,  14,  0, 0, 0, 0, 0},
    { "4KHz",  100,  24,  0, 0, 0, 0, 0},
    { "8KHz",  100,  48,  0, 0, 0, 0, 0},
    { "16KHz", 100,  155, 0, 0, 0, 0, 0}
};

/* store bandwidth variations when sample rate changes */
int lbandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};
int rbandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

} // namespace

SoundStereoEffect::SoundStereoEffect()
{
    effectName = "Sound Stereo spectrometer";

    adc1_config_width(ADC_WIDTH_12Bit);   //Range 0-1023
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db); //ADC_ATTEN_DB_11 = 0-3,6V
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_11db); //ADC_ATTEN_DB_11 = 0-3,6V
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    delay(1000);
    setBandwidth();
}

void SoundStereoEffect::tick()
{
    captureSoundSample();
    renderSpectrometer();
}

void SoundStereoEffect::displayLBand(int band)
{
    int dsize = laudiospectrum[band].curval;
    int dmax = height - 1;
    int ssize = dsize;
    int fsize = dsize / laudiospectrum[band].amplitude;
    double factor = settings->effectScale / 100.0;
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
    int dmax = height - 1;
    int ssize = dsize;
    int fsize = dsize / raudiospectrum[band].amplitude;
    double factor = settings->effectScale / 100.0;
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

void SoundStereoEffect::captureSoundSample()
{
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
        if (adcread) {
            lvReal[i] = adc1_get_raw( ADC1_CHANNEL_0 ); // A raw conversion takes about 20uS on an ESP32
            rvReal[i] = adc1_get_raw( ADC1_CHANNEL_3 ); // A raw conversion takes about 20uS on an ESP32
            delayMicroseconds(20);
        } else {
            lvReal[i] = analogRead(A0); // A conversion takes about 1uS on an ESP32
            rvReal[i] = analogRead(A3); // A conversion takes about 1uS on an ESP32
        }

        lvImag[i] = 0;
        rvImag[i] = 0;

        while ((micros() - newTime) < sampling_period_us) {
            // do nothing to wait
            yield();
        }
    }

    FFT.Windowing(lvReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(lvReal, lvImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(lvReal, lvImag, SAMPLES);

    FFT.Windowing(rvReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(rvReal, rvImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(lvReal, rvImag, SAMPLES);
}

void SoundStereoEffect::renderSpectrometer()
{
    myMatrix->clear();
    for (int i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (lvReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getLBand(i);
            if (bandNum != bands) {
                laudiospectrum[bandNum].curval = (int)lvReal[i];
                displayLBand(bandNum);
            }
        }
        if (rvReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getRBand(i);
            if (bandNum != bands) {
                raudiospectrum[bandNum].curval = (int)rvReal[i];
                displayRBand(bandNum);
            }
        }
    }
}

