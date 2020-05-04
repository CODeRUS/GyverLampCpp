#pragma once
#include "effects/Effect.h"

class Fire18Effect : public Effect
{
public:
    Fire18Effect();
    void activate() override;
    void deactivate() override;
    void tick() override;
};

