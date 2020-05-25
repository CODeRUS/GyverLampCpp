#include "RainbowNoiseEffect.h"

RainbowNoiseEffect::RainbowNoiseEffect(const String &id)
    : NoiseEffect(id)
{
    currentPalette = RainbowColors_p;
}
