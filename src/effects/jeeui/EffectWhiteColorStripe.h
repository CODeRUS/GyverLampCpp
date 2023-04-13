#pragma once
#include "EffectCalc.h"

class EffectWhiteColorStripe : public EffectCalc
{
public:
    EffectWhiteColorStripe(const String &id);

    // EffectCalc interface
    bool run() override;

private:
    bool whiteColorStripeRoutine();
};

