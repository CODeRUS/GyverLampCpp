#pragma once
#include "effects/Effect.h"

class MatrixEffect : public Effect
{
public:
    MatrixEffect();
    void tick() override;
};

