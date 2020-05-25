#pragma once
#include "effects/Effect.h"

class Matrix2Effect : public Effect
{
public:
    explicit Matrix2Effect(const String &id);
    void tick() override;
};

