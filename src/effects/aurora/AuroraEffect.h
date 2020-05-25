#pragma once
#include "effects/Effect.h"

class Boid;
class AuroraEffect : public Effect
{
public:
    explicit AuroraEffect(const String &id);
    void activate() override;
    void deactivate() override;

protected:
    Boid* boids = nullptr;
    uint8_t availableBoidCount = 20;
};

