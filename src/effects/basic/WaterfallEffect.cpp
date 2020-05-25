#include "WaterfallEffect.h"

namespace
{

const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {
  0x000000, 0x060707, 0x101110, 0x151717,
  0x1C1D22, 0x242A28, 0x363B3A, 0x313634,
  0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1,
  0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9
};

int8_t cooling = 32;
int8_t sparkling = 80;

uint8_t** noise3d = nullptr;

}

WaterfallEffect::WaterfallEffect(const String &id)
    : Effect(id)
{

}

void WaterfallEffect::activate()
{
    noise3d = new uint8_t*[mySettings->matrixSettings.width]();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; ++i) {
        noise3d[i] = new uint8_t[mySettings->matrixSettings.height];
    }
}

void WaterfallEffect::deactivate()
{
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        delete[] noise3d[i];
    }
    delete[] noise3d;
}

void WaterfallEffect::tick()
{
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        // Step 1.  Cool down every cell a little
        for (uint8_t i = 0; i < mySettings->matrixSettings.height; i++) {
            noise3d[x][i] = qsub8(noise3d[x][i],
                                  random8(0, ((cooling * 10) / mySettings->matrixSettings.height) + 2));
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (uint8_t k = mySettings->matrixSettings.height - 1; k >= 2; k--) {
            noise3d[x][k] = (noise3d[x][k - 1] + noise3d[x][k - 2] + noise3d[x][k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if (random8() < sparkling) {
            uint8_t y = random8(2);
            noise3d[x][y] = qadd8(noise3d[x][y], random8(160, 255));
        }

        // Step 4.  Map from heat cells to LED colors
        for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
            // Scale the heat value from 0-255 down to 0-240
            // for best results with color palettes.
            uint8_t colorindex = scale8(noise3d[x][j], 240);
            if (settings.scale == 100)
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(WaterfallColors_p, colorindex));
            else
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(CRGBPalette16(CRGB::Black,
                                                                     CHSV(settings.scale * 2.57, 255, 255),
                                                                     CHSV(settings.scale * 2.57, 128, 255),
                                                                     CRGB::White),
                                                       colorindex));// 2.57 вместо 2.55, потому что 100 для белого цвета
            //leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(fire_water ? HeatColors_p : OceanColors_p, colorindex);
        }
    }
}

void WaterfallEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("cooling"))) {
        cooling = json[F("cooling")];
    }
    if (json.containsKey(F("sparkling"))) {
        sparkling = json[F("sparkling")];
    }
}

void WaterfallEffect::writeSettings(JsonObject &json)
{
    json[F("cooling")] = cooling;
    json[F("sparkling")] = sparkling;
}
