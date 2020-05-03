#include "RainEffect.h"

RainEffect::RainEffect()
{

}

void RainEffect::tick()
{
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        // заполняем случайно верхнюю строку
        CRGB thisColor = myMatrix->getPixColorXY(x, mySettings->matrixSettings.height - 1);
        if (!thisColor) {
            if (random8(0, 50) == 0) {
                if (settings.scale <= 1) {
                    myMatrix->drawPixelXY(x,
                                          mySettings->matrixSettings.height - 1,
                                          CHSV(random(0, 9) * 28, 255U, 255U));
                } else if (settings.scale == 100) {
                    myMatrix->drawPixelXY(x,
                                          mySettings->matrixSettings.height - 1,
                                          CRGB(0xE0FFFF - 0x101010 * random(0, 4)));
                } else {
                    myMatrix->drawPixelXY(x,
                                          mySettings->matrixSettings.height - 1,
                                          CHSV(settings.scale * 2.4 + random(0, 16), 255, 255));
                }
            }
        } else {
            myMatrix->shadePixelXY(x,
                                  mySettings->matrixSettings.height - 1,
                                  CHSV(0, 0, random(96, 128)));
        }
    }
    // сдвигаем всё вниз
    for (uint8_t x = 0U; x < mySettings->matrixSettings.width; x++)
    {
        for (uint8_t y = 0U; y < mySettings->matrixSettings.height - 1; y++)
        {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x, y + 1));
        }
    }
}
