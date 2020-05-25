#pragma once
#include "effects/Effect.h"

class SparklesEffect : public Effect
{
public:
    explicit SparklesEffect(const String &id);
    void tick() override;

private:
    void fader(uint8_t step);
};

