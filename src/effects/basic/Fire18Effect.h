#pragma once
#include "effects/Effect.h"

class Fire18Effect : public Effect
{
public:
    explicit Fire18Effect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;
};

