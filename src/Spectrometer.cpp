#include "Spectrometer.h"

#include <math.h>
#if defined(ESP32)
#include <driver/adc.h>
#endif
#include <arduinoFFT.h>

namespace {

Spectrometer *object = nullptr;
uint32_t timer = 0;
uint32_t timeout = 1;

#if defined(ESP32)
adc1_channel_t channel = ADC1_CHANNEL_0;
double noiseFilter = 512;
#else
double noiseFilter = 32;
#endif

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

unsigned long newTime;

Spectrometer::eqBand audiospectrum[EQBANDS] = {
/*
 Adjust the amplitude/bandWidth values
 to fit your microphone
*/
#if defined(ESP8266)
    { 200,  7,   0, 0, 0, 0, 0}, // 250
    { 250,  15,  0, 0, 0, 0, 0}, // 500
    { 220,  20,  0, 0, 0, 0, 0}, // 750
    { 200,  30,  0, 0, 0, 0, 0}, // 1k
    { 175,  39,  0, 0, 0, 0, 0}, // 1.5k
    { 150,  54,  0, 0, 0, 0, 0}, // 2k
    { 125,  85,  0, 0, 0, 0, 0}, // 3k
    { 100,  255, 0, 0, 0, 0, 0}, // 4k
#else
    { 100,  3,   0, 0, 0, 0, 0}, // 250
    { 250,  8,   0, 0, 0, 0, 0}, // 500
    { 220,  14,  0, 0, 0, 0, 0}, // 750
    { 200,  18,  0, 0, 0, 0, 0}, // 1k
    { 175,  26,  0, 0, 0, 0, 0}, // 1.5k
    { 150,  34,  0, 0, 0, 0, 0}, // 2k
    { 125,  56,  0, 0, 0, 0, 0}, // 3k
    { 100,  255, 0, 0, 0, 0, 0}, // 4k
#endif
};

/* store bandwidth variations when sample rate changes */
int bandWidth[EQBANDS] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t hue = 0;


uint8_t GetBand(uint16_t i)
{
    for (uint8_t j = 0; j < EQBANDS; j++) {
        if (i <= bandWidth[j]) {
            return j;
        }
    }
    return EQBANDS;
}

void SetBandwidth()
{
    uint8_t multiplier = SAMPLES / 256;
    for (uint8_t j = 0; j < EQBANDS; j++) {
        bandWidth[j] = audiospectrum[j].bandWidth * multiplier + bandWidth[j - 1];
    }
}

} // namespace

Spectrometer *Spectrometer::instance()
{
    return object;
}

void Spectrometer::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing Spectrometer"));
    object = new Spectrometer;
}

void Spectrometer::loop()
{
    if (timer != 0 && (millis() - timer) < timeout) {
        return;
    }
    timer = millis();

    double* vReal = new double[SAMPLES]();
    double* vImag = new double[SAMPLES]();

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

    delete[] vImag;

    uint16_t readBands[EQBANDS] = {0};
    for (uint16_t i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > noiseFilter) { // Add a crude noise filter, 10 x amplitude or more
            uint8_t bandNum = GetBand(i);
            uint16_t read = static_cast<uint16_t>(vReal[i]);
            if (bandNum != bands && readBands[bandNum] < read) {
                readBands[bandNum] = read;
            }
        }
    }

    delete[] vReal;

//    Serial.print(F("S: "));
    hue = 0;
    for (int bandNum = 0; bandNum < EQBANDS; bandNum++) {
        if (readBands[bandNum] > 0 && audiospectrum[bandNum].curval != readBands[bandNum]) {
            audiospectrum[bandNum].curval = readBands[bandNum];
        }

        uint8_t dmax = 32;
        uint16_t dsize = audiospectrum[bandNum].curval;
#if defined(ESP32)
        uint16_t fsize = dsize / audiospectrum[bandNum].amplitude;
        double factor = 20 / 100.0;
        dsize = fsize * factor;
#else
        dsize = dsize / audiospectrum[bandNum].amplitude;
#endif
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

Spectrometer::eqBand Spectrometer::band(uint8_t i)
{
    return audiospectrum[i];
}

uint8_t Spectrometer::asHue()
{
    return hue;
}

Spectrometer::Spectrometer()
{
#if defined(ESP32)
//    adc1_config_width(ADC_WIDTH_BIT_12);   //Range 0-1023
//    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
#endif
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    SetBandwidth();
    delay(1000);
}
