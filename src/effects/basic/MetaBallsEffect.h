#pragma once
#include "effects/Effect.h"

class MetaBallsEffect : public Effect
{
public:
    explicit MetaBallsEffect(const String &id);
    void tick() override;
};

