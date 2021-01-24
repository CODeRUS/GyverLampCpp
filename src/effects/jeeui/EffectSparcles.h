#pragma once
#include "EffectCalc.h"

class EffectSparcles : public EffectCalc
{
public:
    EffectSparcles(const String &id);

public:
    // EffectCalc interface
    bool run() override;
    String getCtrlVal(uint8_t idx) override;

    // Effect interface
    void initialize(const JsonObject &json) override;
    void writeSettings(JsonObject &json) override;

private:
    bool sparklesRoutine();
};

