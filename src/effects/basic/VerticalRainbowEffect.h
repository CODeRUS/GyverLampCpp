#pragma once
#include "effects/Effect.h"

class VerticalRainbowEffect : public Effect
{
public:
    explicit VerticalRainbowEffect(const String &id);
    void tick() override;
};

