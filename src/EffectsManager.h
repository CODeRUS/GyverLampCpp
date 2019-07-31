#pragma once
#include <Arduino.h>

class EffectsManager
{
public:
    static void Initialize();
    static void Process();

    static void Next();
    static void Previous();
    static void ChangeEffect(uint8_t index);
    static void ActivateEffect(uint8_t index);

    static uint8_t Count();
    static String EffectName(uint8_t index);
};

