#pragma once
#include "EffectCalc.h"

class EffectMatrix : public EffectCalc
{
public:
    EffectMatrix(const String &id);

    // Effect interface
    void activate();
    void deactivate();
    void initialize(const JsonObject &json);
    void writeSettings(JsonObject &json);

    // EffectCalc interface
    bool run();

private:
    bool matrixRoutine();
};

