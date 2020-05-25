#pragma once
#include "effects/Effect.h"

class SinusoidEffect : public Effect
{
public:
    explicit SinusoidEffect(const String &id);
    void tick() override;
};

