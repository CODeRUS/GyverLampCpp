#include "EffectMath.h"
#include "MyMatrix.h"

EffectMath::EffectMath()
{

}

void EffectMath::drawPixelXY(int16_t x, int16_t y, const CRGB &color)
{
    myMatrix->drawPixelXY(x, y, color);
}

void EffectMath::blur2d(uint8_t val)
{
    myMatrix->applyBlur2d(val);
}

void EffectMath::drawCircle(int x0, int y0, int radius, const CRGB &color)
{
    int a = radius, b = 0;
    int radiusError = 1 - a;

    if (radius == 0) {
        EffectMath::drawPixelXY(x0, y0, color);
        return;
    }

    while (a >= b)  {
        EffectMath::drawPixelXY(a + x0, b + y0, color);
        EffectMath::drawPixelXY(b + x0, a + y0, color);
        EffectMath::drawPixelXY(-a + x0, b + y0, color);
        EffectMath::drawPixelXY(-b + x0, a + y0, color);
        EffectMath::drawPixelXY(-a + x0, -b + y0, color);
        EffectMath::drawPixelXY(-b + x0, -a + y0, color);
        EffectMath::drawPixelXY(a + x0, -b + y0, color);
        EffectMath::drawPixelXY(b + x0, -a + y0, color);
        b++;
        if (radiusError < 0)
            radiusError += 2 * b + 1;
        else
        {
            a--;
            radiusError += 2 * (b - a + 1);
        }
    }
}

void EffectMath::fader(uint8_t step)
{
    myMatrix->fadeToBlackBy(step);
}

uint32_t EffectMath::getPixColorXY(uint16_t x, uint16_t y)
{
    return myMatrix->colorcode(myMatrix->getPixColorXY(x, y));
}

void EffectMath::setLed(uint16_t idx, CRGB val)
{
    myMatrix->setLed(idx, val);
}
