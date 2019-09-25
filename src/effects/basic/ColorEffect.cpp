#include "ColorEffect.h"

ColorEffect::ColorEffect()
{
}

void ColorEffect::tick()
{
    myMatrix->fill(CHSV(scale() * 2.5, 255, 255));
}
