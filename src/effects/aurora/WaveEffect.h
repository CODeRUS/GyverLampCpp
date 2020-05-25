#pragma once
#include "effects/Effect.h"

class WaveEffect : public Effect
{
public:
    explicit WaveEffect(const String &id);
    void activate() override;
    void tick() override;
};

