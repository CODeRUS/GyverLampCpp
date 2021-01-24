#pragma once
#include "EffectCalc.h"

class EffectEverythingFall : public EffectCalc
{
public:
    EffectEverythingFall(const String &id);

    // EffectCalc interface
    bool run() override;

    // Effect interface
    void activate() override;
    void deactivate() override;
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;
};

