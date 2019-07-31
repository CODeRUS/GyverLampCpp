#include "SparklesEffect.h"

namespace {

void fadePixel(uint8_t i, uint8_t j, uint8_t step) {     // новый фейдер
    const uint16_t pixelNum = myMatrix->getPixelNumber(i, j);
    const CRGB color = myMatrix->getPixColor(pixelNum);

    if (!color) {
        return;
    }

    if (color.r >= 30 || color.g >= 30 || color.b >= 30) {
        myMatrix->fadeToBlackBy(pixelNum, step);
    } else {
        myMatrix->setLed(pixelNum, CRGB::Black);
    }
}

} // namespace

SparklesEffect::SparklesEffect()
{
    effectName = "Sparkles";
}

void SparklesEffect::tick()
{
    for (uint8_t i = 0; i < settings->effectScale; i++) {
        uint8_t x = random(0, width);
        uint8_t y = random(0, height);
        if (!myMatrix->getPixColorXY(x, y)) {
            myMatrix->setLed(x, y, CHSV(random(0, 255), 255, 255));
        }
    }
    fader(70);
}

void SparklesEffect::fader(uint8_t step) {
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            fadePixel(i, j, step);
        }
    }
}
