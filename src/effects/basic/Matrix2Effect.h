#pragma once
#include "effects/Effect.h"

class Matrix2Effect : public Effect
{
public:
    Matrix2Effect();
    void tick() override;
};

