#pragma once
#include "EffectCalc.h"

class EffectLightBalls : public EffectCalc
{
public:
    EffectLightBalls(const String &id);

    // EffectCalc interface
    bool run();
};

