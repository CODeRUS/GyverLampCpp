#pragma once
#include "MyMatrix.h"
#include "Settings.h"

class Effect
{
public:
    Effect();
    virtual ~Effect();
    void Process();

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void tick() = 0;

    String effectName;
    Settings::EffectSettings *settings = nullptr;

protected:
    uint8_t width = 0;
    uint8_t height = 0;
};
