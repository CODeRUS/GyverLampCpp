#pragma once
#include "EffectCalc.h"

class EffectBall : public EffectCalc
{
public:
    EffectBall(const String &id);

    // Effect interface
    void activate();
    void deactivate();

    // EffectCalc interface
    bool run();
};

