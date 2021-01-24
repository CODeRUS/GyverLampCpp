#include "EffectCalc.h"

EffectCalc::EffectCalc(const String &id)
    : Effect(id)
{
    WIDTH = mySettings->matrixSettings.width;
    HEIGHT = mySettings->matrixSettings.height;
}

bool EffectCalc::run()
{
    return false;
}

String EffectCalc::getCtrlVal(uint8_t idx)
{
    return String();
}

void EffectCalc::tick()
{
    run();
}
