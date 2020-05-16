#pragma once
#include <Arduino.h>

#define mySpectrometer Spectrometer::instance()

class Spectrometer
{
public:
    static Spectrometer *instance();
    static void Initialize();

    struct eqBand {
        uint16_t amplitude;
        uint8_t bandWidth;
        uint16_t peak;
        uint16_t lastpeak;
        uint16_t curval;
        uint16_t lastval;
        uint32_t lastmeasured;
    };

    void loop();
    eqBand band(uint8_t i);

    uint8_t asHue();

protected:
    Spectrometer();
};

