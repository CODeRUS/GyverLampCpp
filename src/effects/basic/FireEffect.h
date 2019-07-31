#pragma once
#include "effects/Effect.h"

class FireEffect : public Effect
{
public:
    FireEffect();
    void tick() override;

private:
    void generateLine();
    void shiftUp();
    void drawFrame(uint8_t pcnt);
};

