#pragma once
#include "effects/Effect.h"

class PrismataEffect : public Effect
{
public:
    explicit PrismataEffect(const String &id);
    void tick() override;
};

