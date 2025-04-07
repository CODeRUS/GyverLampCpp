#pragma once
#include "EffectCalc.h"

class EffectPulse : public EffectCalc
{
public:
    EffectPulse(const String &id);

    // EffectCalc interface
    bool run() override;

private:
    bool pulseRoutine();

    uint8_t pulse_hue;
    uint8_t pulse_step = 0;
    uint8_t centerX = random8(WIDTH - 5U) + 3U;
    uint8_t centerY = random8(HEIGHT - 5U) + 3U;
    uint8_t currentRadius = 4;
    uint8_t _pulse_hue = 0;
    uint8_t _pulse_hueall = 0;
};

