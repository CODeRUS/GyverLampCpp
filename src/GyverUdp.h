#pragma once
#include <Arduino.h>

class GyverUdp
{
public:
    static void Initiazlize(uint16_t port);
    static void Process();
};

