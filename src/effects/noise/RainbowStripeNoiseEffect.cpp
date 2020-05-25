#include "RainbowStripeNoiseEffect.h"

RainbowStripeNoiseEffect::RainbowStripeNoiseEffect(const String &id)
    : NoiseEffect(id)
{
    currentPalette = RainbowStripeColors_p;
}
