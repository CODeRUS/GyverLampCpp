#include "ColorEffect.h"

ColorEffect::ColorEffect()
{
    effectName = "Single color";
}

void ColorEffect::tick()
{
    MyLED::fill(CHSV(settings->effectScale * 2.5, 255, 255));
}
