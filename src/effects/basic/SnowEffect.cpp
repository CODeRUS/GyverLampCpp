#include "SnowEffect.h"

SnowEffect::SnowEffect()
{
    effectName = "Snow";
}

void SnowEffect::tick()
{
    // сдвигаем всё вниз
    for (uint8_t x = 0; x < width; x++) {
        for (byte y = 0; y < height - 1; y++) {
            MyLED::drawPixelXY(x, y, MyLED::getPixColorXY(x, y + 1));
        }
    }

    for (uint8_t x = 0; x < width; x++) {
        // заполняем случайно верхнюю строку
        // а также не даём двум блокам по вертикали вместе быть
        if (!MyLED::getPixColorXY(x, height - 2) && (random(0, settings->effectScale) == 0)) {
            MyLED::drawPixelXY(x, height - 1, 0xE0FFFF - 0x101010 * random(0, 4));
        } else {
            MyLED::drawPixelXY(x, height - 1, 0x000000);
        }
    }
}
