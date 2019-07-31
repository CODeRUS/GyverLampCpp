#pragma once
#include "effects/Effect.h"

class SparklesEffect : public Effect
{
public:
    SparklesEffect();
    void tick() override;

private:
    void fader(uint8_t step);
};

