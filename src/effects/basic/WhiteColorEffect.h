#pragma once
#include "effects/Effect.h"

class WhiteColorEffect : public Effect
{
public:
    explicit WhiteColorEffect(const String &id);
    void tick() override;
};

