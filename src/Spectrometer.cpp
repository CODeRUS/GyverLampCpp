#include "Spectrometer.h"

#include <math.h>
#if defined(ESP32)
#include <driver/adc.h>
#endif
#include <arduinoFFT.h>

namespace {

Spectrometer *instance = nullptr;
uint32_t timer = 0;
uint32_t timeout = 1;

#if defined(ESP32)
adc1_channel_t channel = ADC1_CHANNEL_0;
#endif

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;

Spectrometer::eqBand audiospectrum[EQBANDS] = {
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
int bandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t hue = 0;

} // namespace

Spectrometer *Spectrometer::Instance()
{
    return instance;
}

void Spectrometer::Initialize()
{
    if (instance) {
        return;
    }

    instance = new Spectrometer;
}

void Spectrometer::process()
{
    if (timer != 0 && (millis() - timer) < timeout) {
        return;
    }
    timer = millis();

    captureSoundSample();
    renderSpectrometer();
}

Spectrometer::eqBand Spectrometer::band(uint8_t i)
{
    return audiospectrum[i];
}

uint8_t Spectrometer::asHue()
{
    return hue;
}

void Spectrometer::captureSoundSample()
{
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
#if defined(ESP32)
        vReal[i] = adc1_get_raw(channel); // A raw conversion takes about 20uS on an ESP32
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

    arduinoFFT FFT(vReal, vImag, SAMPLES, samplingFrequency);
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
}

void Spectrometer::renderSpectrometer()
{
    uint16_t readBands[EQBANDS] = {0};
    for (uint16_t i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > 512) { // Add a crude noise filter, 10 x amplitude or more
            uint8_t bandNum = getBand(i);
            uint16_t read = static_cast<uint16_t>(vReal[i]);
            if (bandNum != bands && readBands[bandNum] < read) {
                readBands[bandNum] = read;
            }
        }
    }

//    Serial.print(F("S: "));
    hue = 0;
    for (int bandNum = 0; bandNum < EQBANDS; bandNum++) {
        if (readBands[bandNum] > 0 && audiospectrum[bandNum].curval != readBands[bandNum]) {
            audiospectrum[bandNum].curval = readBands[bandNum];
        }

        uint8_t dmax = 32;
        uint16_t dsize = audiospectrum[bandNum].curval;
        uint16_t fsize = dsize / audiospectrum[bandNum].amplitude;
        double factor = 20 / 100.0;
        dsize = fsize * factor;
        if (dsize > dmax) {
            dsize = dmax;
        }
        if (dsize != audiospectrum[bandNum].lastval) {
            audiospectrum[bandNum].lastval = dsize;
        }

        hue = hue + dsize;
//        Serial.printf_P(PSTR("%05d %02d "), audiospectrum[bandNum].curval, audiospectrum[bandNum].lastval);
    }
//    hue = hue * 8;
//    Serial.print(hue);
//    Serial.println();
}

uint8_t Spectrometer::getBand(uint16_t i)
{
    for (uint8_t j = 0; j < EQBANDS; j++) {
        if (i <= bandWidth[j]) {
            return j;
        }
    }
    return EQBANDS;
}

void Spectrometer::setBandwidth()
{
    uint8_t multiplier = SAMPLES / 256;
    bandWidth[0] = audiospectrum[0].bandWidth * multiplier;
    for (uint8_t j = 1; j < EQBANDS; j++) {
        bandWidth[j] = audiospectrum[j].bandWidth * multiplier + bandWidth[j - 1];
    }
}

Spectrometer::Spectrometer()
{
#if defined(ESP32)
//    adc1_config_width(ADC_WIDTH_BIT_12);   //Range 0-1023
//    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
#endif
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    setBandwidth();
    delay(1000);
}
