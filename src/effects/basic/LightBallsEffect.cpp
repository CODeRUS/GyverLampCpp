#include "LightBallsEffect.h"

namespace {

uint8_t thickness = 1;

uint8_t paintWidth = 0;
uint8_t paintHeight = 0;

}

LightBallsEffect::LightBallsEffect(const String &id)
    : Effect(id)
{

}

void LightBallsEffect::tick()
{
    paintWidth = mySettings->matrixSettings.width - thickness * 2;
    paintHeight = mySettings->matrixSettings.height - thickness * 2;

    // Apply some blurring to whatever's already on the matrix
    // Note that we never actually clear the matrix, we just constantly
    // blur it repeatedly. Since the blurring is 'lossy', there's
    // an automatic trend toward black -- by design.
    uint8_t blurAmount = dim8_raw(beatsin8(3, 64, 100));
    myMatrix->applyBlur2d(blurAmount);

    // Use two out-of-sync sine waves
    uint16_t i = beatsin16( 79, 0, 255); //91
    uint16_t j = beatsin16( 67, 0, 255); //109
    uint16_t k = beatsin16( 53, 0, 255); //73
    uint16_t m = beatsin16( 97, 0, 255); //123

    // The color of each point shifts over time, each at a different speed.
    uint32_t ms = millis() / (settings.scale / 4 + 1);
    myMatrix->drawPixelXY(highByte(i * paintWidth) + thickness,
                          highByte(j * paintHeight) + thickness,
                          CHSV(ms / 29, 200, 255));
    myMatrix->drawPixelXY(highByte(j * paintWidth) + thickness,
                          highByte(k * paintHeight) + thickness,
                          CHSV(ms / 41, 200, 255));
    myMatrix->drawPixelXY(highByte(k * paintWidth) + thickness,
                          highByte(m * paintHeight) + thickness,
                          CHSV(ms / 37, 200, 255));
    myMatrix->drawPixelXY(highByte(m * paintWidth) + thickness,
                          highByte(i * paintHeight) + thickness,
                          CHSV(ms / 53, 200, 255));
}

void LightBallsEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("size"))) {
        thickness = json[F("size")];
    }
}

void LightBallsEffect::writeSettings(JsonObject &json)
{
    json[F("size")] = thickness;
}
