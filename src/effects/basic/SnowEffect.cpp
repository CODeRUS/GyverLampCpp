#include "SnowEffect.h"

SnowEffect::SnowEffect()
{
}

void SnowEffect::tick()
{
    // сдвигаем всё вниз
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (byte y = 0; y < mySettings->matrixSettings.height - 1; y++) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x, y + 1));
        }
    }

    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (!myMatrix->getPixColorXY(x, mySettings->matrixSettings.height - 2) && (random(0, settings.scale) == 0)) {
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, 0xE0FFFF - 0x101010 * random(0, 4));
        } else {
            myMatrix->drawPixelXY(x, mySettings->matrixSettings.height - 1, 0x000000);
        }
    }
}
