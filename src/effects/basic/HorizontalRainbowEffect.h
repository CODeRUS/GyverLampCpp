#pragma once
#include "effects/Effect.h"

class HorizontalRainbowEffect : public Effect
{
public:
    explicit HorizontalRainbowEffect(const String &id);
    void tick() override;
};

