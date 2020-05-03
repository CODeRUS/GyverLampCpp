#pragma once
#include "effects/Effect.h"

class FractionalEffect : public Effect
{
public:
    FractionalEffect(uint8_t layers = 1);
    void activate() override;
    void deactivate() override;

protected:
    void FillNoise(int8_t layer);
    void MoveX(int8_t delta);
    void MoveY(int8_t delta);
    void MoveFractionalNoiseX(int8_t amplitude = 1, float shift = 0);
    void MoveFractionalNoiseY(int8_t amplitude = 1, float shift = 0);

    CRGB* ledsbuff = nullptr;

    int8_t zD;
    int8_t zF;
    uint8_t numLayers = 1;

    // The coordinates for 3 16-bit noise spaces.
    uint32_t* effectX = nullptr;
    uint32_t* effectY = nullptr;
    uint32_t* effectZ = nullptr;
    uint32_t* effectScaleX = nullptr;
    uint32_t* effectScaleY = nullptr;
    uint8_t*** noise3d = nullptr;

    uint8_t eNs_noisesmooth = 0;
    bool eNs_isSetupped = false;
};

