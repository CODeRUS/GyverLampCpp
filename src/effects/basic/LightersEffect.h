#pragma once
#include "effects/Effect.h"

class LightersEffect : public Effect
{
public:
    explicit LightersEffect(const String &id);
    void activate() override;
    void deactivate() override;
    void tick() override;

};

