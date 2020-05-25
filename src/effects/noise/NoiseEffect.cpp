#include "NoiseEffect.h"

NoiseEffect::NoiseEffect(const String &id)
    : Effect(id)
{
}

void NoiseEffect::activate()
{
    maxDimension = max(mySettings->matrixSettings.width, mySettings->matrixSettings.height);
    noise = new uint8_t*[maxDimension]();
    for (uint8_t i = 0; i < maxDimension; ++i) {
        noise[i] = new uint8_t[maxDimension];
    }
    colorLoop = 0;
}

void NoiseEffect::deactivate()
{
    for (uint8_t i = 0; i < maxDimension; i++) {
        delete[] noise[i];
    }
    delete[] noise;
}

void NoiseEffect::tick()
{
    fillNoiseLED();
}

void NoiseEffect::fillNoise8()
{
    for (uint8_t i = 0; i < maxDimension; i++) {
        const uint16_t ioffset = settings.scale * i;
        if (i % 3 == 0) {
#if defined(ESP8266)
            ESP.wdtFeed();
#else
            yield();
#endif
        }
        for (uint8_t j = 0; j < maxDimension; j++) {
            const uint16_t joffset = settings.scale * j;
            const uint8_t val = inoise8(x + ioffset, y + joffset, z);
            noise[i][j] = val;
        }
    }
    z += settings.speed;
}

void NoiseEffect::fillNoiseLED()
{
    uint8_t dataSmoothing = 0;
    if (settings.speed < 50) {
        dataSmoothing = 200 - (settings.speed * 4);
    }
    for (uint8_t i = 0; i < maxDimension; i++) {
        uint16_t ioffset = settings.scale * i;
        if (i % 3 == 0) {
#if defined(ESP8266)
            ESP.wdtFeed();
#else
            yield();
#endif
        }
        for (uint8_t j = 0; j < maxDimension; j++) {
            uint16_t joffset = settings.scale * j;

            uint8_t data = inoise8(x + ioffset, y + joffset, z);

            data = qsub8(data, 16);
            data = qadd8(data, scale8(data, 39));

            if (dataSmoothing > 0) {
                const uint8_t olddata = noise[i][j];
                const uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
                data = newdata;
            }

            noise[i][j] = data;
        }
    }
    z += settings.speed;

    // apply slow drift to X and Y, just for visual variation.
    x += settings.speed / 8;
    y -= settings.speed / 16;

    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        if (i % 3 == 0) {
#if defined(ESP8266)
            ESP.wdtFeed();
#else
            yield();
#endif
        }
        for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
            uint8_t index = noise[j][i];
            uint8_t bri =   noise[i][j];
            // if this palette is a 'loop', add a slowly-changing base value
            if (colorLoop > 0) {
                index += ihue;
            }
            // brighten up, as the color palette itself often contains the
            // light/dark dynamic range desired
            if (bri > 127) {
                bri = 255;
            } else {
                bri = dim8_raw(bri * 2);
            }
            CRGB color = ColorFromPalette(currentPalette, index, bri);
            myMatrix->drawPixelXY(i, j, color);
        }
    }
    ihue += 1;
}
