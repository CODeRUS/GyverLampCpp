#include "EffectEverythingFall.h"

namespace {

uint8_t SPARKINGNEW = 80;
uint8_t palette = 0;
uint8_t** heat = nullptr;

}

EffectEverythingFall::EffectEverythingFall(const String &id)
    : EffectCalc(id)
{

}

bool EffectEverythingFall::run()
{
    uint8_t coolingnew = map (settings.scale, 1, 100, 93, 10);

    for (uint8_t x = 0; x < WIDTH; x++) {
        // Step 1.  Cool down every cell a little
        for (uint8_t i = 0; i < HEIGHT; i++) {
            heat[x][i] = qsub8(heat[x][i], random(0, coolingnew));
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (unsigned int k = HEIGHT - 1; k >= 2; k--) {
            heat[x][k] = (heat[x][k - 1] + heat[x][k - 2] + heat[x][k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if (random8() < SPARKINGNEW) {
            int y = random(2);
            heat[x][y] = qadd8(heat[x][y], random(160, 255));
        }

        // Step 4.  Map from heat cells to LED colors
        for (uint8_t j = 0; j < HEIGHT; j++) {
            // Scale the heat value from 0-255 down to 0-240
            // for best results with color palettes.
            byte colorindex = scale8(heat[x][(uint8_t)j], 240);
            myMatrix->blendPixelXY(x, (HEIGHT - 1) - j, ColorFromPalette(*myMatrix->GetColorPalette(palette / 2.55f), colorindex /*, heat[x][j]*/), 50);

        }
    }
    return true;
}

void EffectEverythingFall::activate()
{
    heat = new uint8_t*[mySettings->matrixSettings.width]();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; ++i) {
        heat[i] = new uint8_t[mySettings->matrixSettings.height];
    }
}

void EffectEverythingFall::deactivate()
{
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        delete[] heat[i];
    }
    delete[] heat;
}

void EffectEverythingFall::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("sparkling"))) {
        SPARKINGNEW = json[F("sparkling")];
    }
    if (json.containsKey(F("palette"))) {
        palette = json[F("palette")];
    }
}

void EffectEverythingFall::writeSettings(JsonObject &json)
{
    json[F("sparkling")] = SPARKINGNEW;
    json[F("palette")] = palette;
}
