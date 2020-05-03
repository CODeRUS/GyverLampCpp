#include "Fire12Effect.h"

namespace {

uint8_t cooling = 70;
uint8_t sparking = 130;
uint8_t fireSmoothing = 80;

uint8_t fireBase = 3;
uint8_t** noise3d = nullptr;

}

Fire12Effect::Fire12Effect()
{

}

void Fire12Effect::activate()
{
    random16_add_entropy(random(256));
    fireBase = mySettings->matrixSettings.height / 6 + 1;

    noise3d = new uint8_t*[mySettings->matrixSettings.width]();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; ++i) {
        noise3d[i] = new uint8_t[mySettings->matrixSettings.height];
    }
}

void Fire12Effect::deactivate()
{
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        delete[] noise3d[i];
    }
    delete[] noise3d;
}

void Fire12Effect::tick()
{
    // Loop for each column individually
    for (int x = 0; x < mySettings->matrixSettings.width; x++) {
        // Step 1.  Cool down every cell a little
        for (int i = 0; i < mySettings->matrixSettings.height; i++) {
            noise3d[x][i] = qsub8(noise3d[x][i],
                                  random(0, ((cooling * 10) / mySettings->matrixSettings.height) + 2));
        }
        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = mySettings->matrixSettings.height; k > 1; k--) {
            noise3d[x][myMatrix->wrapY(k)] = (noise3d[x][k - 1] + noise3d[x][myMatrix->wrapY(k - 2)] + noise3d[x][myMatrix->wrapY(k - 2)]) / 3;
        }
        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if (random8() < sparking) {
            int j = random(fireBase);
            noise3d[x][j] = qadd8(noise3d[x][j], random(160, 255));
        }
        // Step 4.  Map from heat cells to LED colors
        if (settings.scale > 50) {
            for (int y = 0; y < mySettings->matrixSettings.height; y++) {
                // Blend new data with previous frame. Average data between neighbouring pixels
                myMatrix->blendPixelXY(x, y, ColorFromPalette(*myMatrix->GetFirePalette((settings.scale - 50) * 2), ((noise3d[x][y] * 0.7f) + (noise3d[myMatrix->wrapX(x + 1)][y] * 0.3f))), fireSmoothing);
            }
        } else {
            for (int y = 0; y < mySettings->matrixSettings.height; y++) {
                // Blend new data with previous frame. Average data between neighbouring pixels
                myMatrix->blendPixelXY(x, y, ColorFromPalette(*myMatrix->GetColorPalette(settings.scale * 2), ((noise3d[x][y] * 0.7f) + (noise3d[myMatrix->wrapX(x + 1)][y] * 0.3f))), fireSmoothing);
            }
        }
    }
}

void Fire12Effect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("cooling"))) {
        cooling = json[F("cooling")];
    }
    if (json.containsKey(F("sparking"))) {
        sparking = json[F("sparking")];
    }
    if (json.containsKey(F("fireSmoothing"))) {
        fireSmoothing = json[F("fireSmoothing")];
    }
}

void Fire12Effect::writeSettings(JsonObject &json)
{
    json[F("cooling")] = cooling;
    json[F("sparking")] = sparking;
    json[F("fireSmoothing")] = fireSmoothing;
}
