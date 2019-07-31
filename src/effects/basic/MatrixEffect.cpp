#include "MatrixEffect.h"

namespace  {

CRGB mainColor = 0x00FF00;
CRGB fadeColor = 0x002000;

} // namespace

MatrixEffect::MatrixEffect()
{
    effectName = "Matrix";
}

void MatrixEffect::tick()
{
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height - 1; y++) {
            MyLED::drawPixelXY(x, y, MyLED::getPixColorXY(x, y + 1));
        }

        // заполняем случайно верхнюю строку
        CRGB thisColor = MyLED::getPixColorXY(x, height - 1);
        if (!thisColor) {
            if (random(0, settings->effectScale) == 0) {
                MyLED::drawPixelXY(x, height - 1, mainColor);
            }
        } else if (thisColor < fadeColor) {
            MyLED::drawPixelXY(x, height - 1, 0);
        } else {
            MyLED::drawPixelXY(x, height - 1, thisColor - fadeColor);
        }
    }
}
