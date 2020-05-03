#pragma once
#include "effects/Effect.h"

class Boid;
class AuroraEffect : public Effect
{
public:
    AuroraEffect();
    void activate() override;
    void deactivate() override;

protected:
    Boid* boids = nullptr;
    uint8_t availableBoidCount = 20;
};

