#include "ColorsEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

ColorsEffect::ColorsEffect()
{
}

void ColorsEffect::tick()
{
    hue += scale();
    myMatrix->fill(CHSV(hue, 255, 255));
}
