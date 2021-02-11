#include "EffectMath.h"
#include "MyMatrix.h"

namespace {

uint8_t WU_WEIGHT(uint8_t a, uint8_t b)
{
    return (a*b + a+b) >> 8;
}

}

EffectMath::EffectMath()
{

}

void EffectMath::drawPixelXY(uint8_t x, uint8_t y, const CRGB &color)
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

uint32_t EffectMath::getPixColorXY(uint8_t x, uint8_t y)
{
    return myMatrix->colorcode(myMatrix->getPixColorXY(x, y));
}

void EffectMath::setLed(uint16_t idx, CRGB val)
{
    myMatrix->setLed(idx, val);
}

float EffectMath::randomf(float min, float max)
{
    return fmap<float>((float)random16(4095), 0.0f, 4095.0f, min, max);
}

void EffectMath::dimAll(uint8_t value)
{
    myMatrix->dimAll(value);
}

void EffectMath::drawPixelXYF_Y(uint16_t x, float y, const CRGB &color, uint8_t darklevel)
{
    if (x<0 || y<0 || x>((float)myMatrix->width()) || y>((float)myMatrix->height())) return;

    // extract the fractional parts and derive their inverses
    uint8_t yy = (y - (int)y) * 255, iy = 255 - yy;
    // calculate the intensities for each affected pixel
    uint8_t wu[2] = {iy, yy};
    // multiply the intensities by the colour, and saturating-add them to the pixels
    for (int8_t i = 1; i >= 0; i--) {
        uint8_t yn = y + (i & 1);
        CRGB clr = EffectMath::getPixColorXY(x, yn);
        if (yn>0 && yn<(int)myMatrix->height()-1) {
            clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
            clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
            clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
        } else if(yn==0 || yn==(int)myMatrix->height()-1) {
            clr.r = qadd8(clr.r, (color.r * 85) >> 8);
            clr.g = qadd8(clr.g, (color.g * 85) >> 8);
            clr.b = qadd8(clr.b, (color.b * 85) >> 8);
        }
        if (darklevel > 0) {
            EffectMath::drawPixelXY(x, yn, EffectMath::makeDarker(clr, darklevel));
        } else {
            EffectMath::drawPixelXY(x, yn, clr);
        }
    }
}

void EffectMath::drawPixelXYF(float x, float y, const CRGB &color, uint8_t darklevel)
{
    if (x<0 || y<0 || x>((float)myMatrix->width()) || y>((float)myMatrix->height())) return;

    // extract the fractional parts and derive their inverses
    uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx, iy = 255 - yy;
    // calculate the intensities for each affected pixel
    uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy),
                     WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)};
    // multiply the intensities by the colour, and saturating-add them to the pixels
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t xn = x + (i & 1);
        uint8_t yn = y + ((i >> 1) & 1);
        CRGB clr = EffectMath::getPixColorXY(xn, yn);
        clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
        clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
        clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);

        // if(xn<(int)WIDTH-1 && yn<(int)HEIGHT-1 && yn>0 && xn>0){
        //   clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
        //   clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
        //   clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
        // } else if((yn==0 || yn==HEIGHT-1 || xn==0) && xx<127) {
        //   clr.r = qadd8(clr.r, (color.r * 64) >> 8);
        //   clr.g = qadd8(clr.g, (color.g * 64) >> 8);
        //   clr.b = qadd8(clr.b, (color.b * 64) >> 8);
        // }
        if (darklevel > 0) {
            EffectMath::drawPixelXY(xn, yn, EffectMath::makeDarker(clr, darklevel));
        } else {
            EffectMath::drawPixelXY(xn, yn, clr);
        }
    }
}

CRGB EffectMath::makeDarker(const CRGB &color, fract8 howMuchDarker)
{
    CRGB newcolor = color;
    newcolor.nscale8(255 - howMuchDarker);
    return newcolor;
}

template <typename T>
T EffectMath::fmap(const T x, const T in_min, const T in_max, const T out_min, const T out_max)
{
    return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}
