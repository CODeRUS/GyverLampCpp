#pragma once
#include "effects/Effect.h"

class NoiseEffect : public Effect
{
public:
    explicit NoiseEffect(const String &id);
    void activate() override;
    void deactivate() override;
    virtual void tick() override;

protected:
    void fillNoise8();
    void fillNoiseLED();

    uint8_t maxDimension = 0;

    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t z = 0;

    uint8_t** noise = nullptr;

    CRGBPalette16 currentPalette;
    uint8_t colorLoop = 1;
    uint8_t ihue = 0;

};

