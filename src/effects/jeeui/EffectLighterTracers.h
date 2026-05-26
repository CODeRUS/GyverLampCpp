#pragma once
#include "EffectCalc.h"

class EffectLighterTracers : public EffectCalc
{
public:
    EffectLighterTracers(const String &id);

    // EffectCalc interface
    bool run();

    // Effect interface
    void activate();
    void deactivate();

private:
    bool lighterTracersRoutine();
};

