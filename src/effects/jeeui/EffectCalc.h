#pragma once
#include "../Effect.h"
#include "EffectMath.h"

#define BRIGHTNESS mySettings->matrixSettings.maxBrightness

class EffectCalc : public Effect
{
public:
    EffectCalc(const String &id);

    virtual bool run();
    virtual String getCtrlVal(uint8_t idx);

    // Effect interface
    void tick() override;

    uint8_t WIDTH = 0;
    uint8_t HEIGHT = 0;
};

