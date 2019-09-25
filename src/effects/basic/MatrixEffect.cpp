#include "MatrixEffect.h"

namespace  {

CRGB mainColor = 0x00FF00;
CRGB fadeColor = 0x002000;

} // namespace

MatrixEffect::MatrixEffect()
{
}

void MatrixEffect::tick()
{
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height - 1; y++) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x, y + 1));
        }

        // заполняем случайно верхнюю строку
        CRGB thisColor = myMatrix->getPixColorXY(x, height - 1);
        if (!thisColor) {
            if (random(0, scale()) == 0) {
                myMatrix->drawPixelXY(x, height - 1, mainColor);
            }
        } else if (thisColor < fadeColor) {
            myMatrix->drawPixelXY(x, height - 1, 0);
        } else {
            myMatrix->drawPixelXY(x, height - 1, thisColor - fadeColor);
        }
    }
}
