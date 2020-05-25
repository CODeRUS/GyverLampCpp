#include "SoundEffect.h"

#include <math.h>
#if defined(ESP32)
#include <driver/adc.h>
#endif
#include <arduinoFFT.h>
#include <limits.h>

namespace  {

bool heatColor = true;
uint32_t color = CRGB::Blue;

#if defined(ESP32)
adc1_channel_t channel = ADC1_CHANNEL_0;
double noiseFilter = 256;
#else
double noiseFilter = 0;
#endif

#define SAMPLES 256
int samplingFrequency = 40000;
#define EQBANDS 8
uint16_t bands = EQBANDS;

unsigned int sampling_period_us;

unsigned long newTime;

struct eqBand {
    uint16_t amplitude;
    uint8_t bandWidth;
    int peak;
    int lastpeak;
    int curval;
    int lastval;
    unsigned long lastmeasured;
};

eqBand audiospectrum[EQBANDS] = {
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

} // namespace

SoundEffect::SoundEffect(const String &id)
    : Effect(id)
{
#if defined(ESP32)
    adc1_config_width(ADC_WIDTH_BIT_12);   //Range 0-1023
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
#endif
    sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
    delay(1000);
    setBandwidth();
}

void SoundEffect::tick()
{
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

    myMatrix->clear();
    int readBands[EQBANDS] = {INT_MAX};
    for (int i = 2; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > noiseFilter) { // Add a crude noise filter, 10 x amplitude or more
            byte bandNum = getBand(i);
            int read = (int)vReal[i];
            if (bandNum != bands && readBands[bandNum] < read) {
                readBands[bandNum] = read;
            }
        }
    }

    delete[] vReal;

//    Serial.print(F("E: "));
    for (int bandNum = 0; bandNum < EQBANDS; bandNum++) {
        if (readBands[bandNum] < INT_MAX && audiospectrum[bandNum].curval != readBands[bandNum]) {
            audiospectrum[bandNum].curval = readBands[bandNum];
        }
        displayBand(bandNum, audiospectrum[bandNum].curval);

//        Serial.printf_P(PSTR("%05d %02d "), audiospectrum[bandNum].curval, audiospectrum[bandNum].lastval);
    }
//    Serial.println();


}

void SoundEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("color"))) {
        color = json[F("color")];
    }
    if (json.containsKey(F("hColor"))) {
        heatColor = json[F("hColor")];
    }
}

void SoundEffect::writeSettings(JsonObject &json)
{
    json[F("color")] = color;
    json[F("hColor")] = heatColor;
}

void SoundEffect::displayBand(int band, int dsize)
{
    int dmax = mySettings->matrixSettings.height;
//    int ssize = dsize;
    int fsize = dsize / audiospectrum[band].amplitude;
    double factor = settings.scale / 100.0;
    dsize = fsize * factor;
//    Serial.printf("displayBand %d, %d, %d, %f, %d\n", band, ssize, fsize, factor, dsize);
    if (dsize > dmax) {
        dsize = dmax;
    }
    for (int y = 0; y < dsize; y++) {
        CRGB pixColor;
        if (heatColor) {
            uint8_t r = (y + 1) * 16 - 1;
            uint8_t b = 255 - ((y + 1) * 16 - 1);
            pixColor = CRGB(r, 0, b);
        } else {
            pixColor = CRGB(color);
        }
        myMatrix->drawPixelXY(band * 2, y, pixColor);
        myMatrix->drawPixelXY(band * 2 + 1, y, pixColor);
    }
    audiospectrum[band].lastval = dsize;
    audiospectrum[band].lastmeasured = millis();

//    Serial.printf_P(PSTR("E%d %05d %02d\n"), band, audiospectrum[band].curval, dsize);
}

void SoundEffect::setBandwidth()
{
    byte multiplier = SAMPLES / 256;
    for (byte j = 0; j < EQBANDS; j++) {
        bandWidth[j] = audiospectrum[j].bandWidth * multiplier;
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

