#pragma once
#include "effects/Effect.h"

class DiagonalRainbowEffect : public Effect
{
public:
    explicit DiagonalRainbowEffect(const String &id);
    void tick() override;
};

