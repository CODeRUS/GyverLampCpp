#include "MatrixEffect.h"

namespace  {

CRGB mainColor = 0x00FF00;
CRGB fadeColor = 0x002000;

} // namespace

MatrixEffect::MatrixEffect(const String &id)
    : Effect(id)
{
}

void MatrixEffect::tick()
{
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x, y + 1));
        }

        // заполняем случайно верхнюю строку
        CRGB thisColor = myMatrix->getPixColorXY(x, mySettings->matrixSettings.height - 1);
        if (!thisColor) {
            if (random(0, settings.scale) == 0) {
                myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, mainColor);
            }
        } else if (thisColor < fadeColor) {
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, 0);
        } else {
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, thisColor - fadeColor);
        }
    }
}
