#include "CloudNoiseEffect.h"

CloudNoiseEffect::CloudNoiseEffect(const String &id)
    : NoiseEffect(id)
{
    currentPalette = CloudColors_p;
}
