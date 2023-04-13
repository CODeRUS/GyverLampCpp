#pragma once
#include "EffectCalc.h"

class EffectLighters : public EffectCalc
{
public:
    EffectLighters(const String &id);

    // Effect interface
    void activate();
    void deactivate();
    void initialize(const JsonObject &json);
    void writeSettings(JsonObject &json);

    // EffectCalc interface
    bool run();

private:
    bool lightersRoutine();
};

