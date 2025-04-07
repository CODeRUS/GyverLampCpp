#pragma once
#include "EffectCalc.h"

class EffectStarFall : public EffectCalc
{
public:
    EffectStarFall(const String &id);

    // Effect interface
    void activate();
    void deactivate();
    void initialize(const JsonObject &json);
    void writeSettings(JsonObject &json);

    // EffectCalc interface
    bool run();

private:
    bool snowStormStarfallRoutine();
};

