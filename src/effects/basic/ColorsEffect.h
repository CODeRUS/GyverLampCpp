#pragma once
#include "effects/Effect.h"

class ColorsEffect : public Effect
{
public:
    explicit ColorsEffect(const String &id);
    void tick() override;
};

