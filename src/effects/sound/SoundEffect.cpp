#include "SoundEffect.h"

#include <math.h>
#if defined(ESP32)
#include <driver/adc.h>
#endif
#include <arduinoFFT.h>

namespace  {

arduinoFFT FFT = arduinoFFT();

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;

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

eqBand audiospectrum[EQBANDS] = {
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
int bandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

} // namespace

SoundEffect::SoundEffect()
{
    effectName = "Sound spectrometer";

#if defined(ESP32)
    adc1_config_width(ADC_WIDTH_12Bit);   //Range 0-1023
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db); //ADC_ATTEN_DB_11 = 0-3,6V
#endif
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    delay(1000);
    setBandwidth();
}

void SoundEffect::tick()
{
    captureSoundSample();
    renderSpectrometer();
}

void SoundEffect::displayBand(int band, int dsize)
{
    int dmax = height - 1;
    int ssize = dsize;
    int fsize = dsize / audiospectrum[band].amplitude;
    double factor = settings->effectScale / 100.0;
    dsize = fsize * factor;
//    Serial.printf("displayBand %d, %d, %d, %f, %d\n", band, ssize, fsize, factor, dsize);
    if (dsize > dmax) {
        dsize = dmax;
    }
    for (int y = 0; y < dsize; y++) {
        myMatrix->drawPixelXY(band * 2, y, CRGB(CRGB::Blue));
        myMatrix->drawPixelXY(band * 2 + 1, y, CRGB(CRGB::Blue));
    }
    if (dsize > audiospectrum[band].peak) {
        audiospectrum[band].peak = dsize;
    }
    audiospectrum[band].lastval = dsize;
    audiospectrum[band].lastmeasured = millis();
}

void SoundEffect::setBandwidth()
{
    byte multiplier = SAMPLES / 256;
    bandWidth[0] = audiospectrum[0].bandWidth * multiplier;
    for (byte j = 1; j < EQBANDS; j++) {
        bandWidth[j] = audiospectrum[j].bandWidth * multiplier + bandWidth[j - 1];
    }
}


byte SoundEffect::getBand(int i)
{
    for (byte j = 0; j < EQBANDS; j++) {
        if (i <= bandWidth[j]) {
            return j;
        }
    }
    return EQBANDS;
}

void SoundEffect::captureSoundSample()
{
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
#if defined(ESP32)
        vReal[i] = adc1_get_raw( ADC1_CHANNEL_0 ); // A raw conversion takes about 20uS on an ESP32
        delayMicroseconds(20);
#else
        vReal[i] = analogRead(A0); // A conversion takes about 1uS on an ESP32
#endif

        vImag[i] = 0;

        while ((micros() - newTime) < sampling_period_us) {
            // do nothing to wait
            yield();
        }
    }

    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
}

void SoundEffect::renderSpectrometer()
{
    myMatrix->clear();
    for (int i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getBand(i);
            if (bandNum != bands) {
                audiospectrum[bandNum].curval = (int)vReal[i];
                displayBand(bandNum, audiospectrum[bandNum].curval);
            }
        }
    }
}

