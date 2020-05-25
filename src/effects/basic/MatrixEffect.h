#pragma once
#include "effects/Effect.h"

class MatrixEffect : public Effect
{
public:
    explicit MatrixEffect(const String &id);
    void tick() override;
};

