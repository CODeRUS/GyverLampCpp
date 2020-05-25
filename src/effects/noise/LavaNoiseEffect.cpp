#include "LavaNoiseEffect.h"

LavaNoiseEffect::LavaNoiseEffect(const String &id)
    : NoiseEffect(id)
{
    currentPalette = LavaColors_p;
}
