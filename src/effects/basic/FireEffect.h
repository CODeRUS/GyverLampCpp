#pragma once
#include "effects/Effect.h"

class FireEffect : public Effect
{
public:
    FireEffect();
    void activate() override;
    void deactivate() override;
    void tick() override;

private:
    void generateLine();
    void shiftUp();
    void drawFrame(uint8_t pcnt);

};

