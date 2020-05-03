#include "SpiralEffect.h"

namespace {

uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255)
{
    uint8_t beatsin = sin8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255)
{
    uint8_t beatcos = cos8(theta);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatcos, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

byte spirotheta1 = 0;
byte spirotheta2 = 0;
uint8_t hue = 0;

uint8_t spiroradiusx = 0;
uint8_t spiroradiusy = 0;

uint8_t spirocenterX = 0;
uint8_t spirocenterY = 0;

uint8_t spirominx = 0;
uint8_t spiromaxx = 0;
uint8_t spirominy = 0;
uint8_t spiromaxy = 0;

uint8_t spirocount = 1;
uint8_t spirooffset = 256;
boolean spiroincrement = false;

boolean spirohandledChange = false;

}

SpiralEffect::SpiralEffect()
{

}

void SpiralEffect::activate()
{
    spiroradiusx = mySettings->matrixSettings.width / 4 - 1;
    spiroradiusy = mySettings->matrixSettings.height / 4 - 1;

    spirocenterX = mySettings->matrixSettings.width / 2;
    spirocenterY = mySettings->matrixSettings.height / 2;

    spirominx = spirocenterX - spiroradiusx;
    spiromaxx = spirocenterX + spiroradiusx + 1;
    spirominy = spirocenterY - spiroradiusy;
    spiromaxy = spirocenterY + spiroradiusy + 1;

    spirooffset = 256 / spirocount;
}

void SpiralEffect::tick()
{
    myMatrix->applyBlur2d(20);
    myMatrix->dimAll(255 - settings.speed / 10);

    boolean change = false;

    for (int i = 0; i < spirocount; i++) {
        uint8_t x = mapsin8(spirotheta1 + i * spirooffset, spirominx, spiromaxx);
        uint8_t y = mapcos8(spirotheta1 + i * spirooffset, spirominy, spiromaxy);

        uint8_t x2 = mapsin8(spirotheta2 + i * spirooffset, x - spiroradiusx, x + spiroradiusx);
        uint8_t y2 = mapcos8(spirotheta2 + i * spirooffset, y - spiroradiusy, y + spiroradiusy);


        if (x2 < mySettings->matrixSettings.width && y2 < mySettings->matrixSettings.height) {
            myMatrix->drawPixelXY(x2,
                                  y2,
                                  ColorFromPalette(*myMatrix->GetPalette(settings.scale),
                                                                        hue + i * spirooffset));
        }

        if ((x2 == spirocenterX && y2 == spirocenterY) ||
            (x2 == spirocenterX && y2 == spirocenterY)) {
            change = true;
        }
    }

    spirotheta2 += 2;

    EVERY_N_MILLIS(12) {
        spirotheta1 += 1;
    }

    EVERY_N_MILLIS(75) {
        if (change && !spirohandledChange) {
            spirohandledChange = true;

            if (spirocount >= mySettings->matrixSettings.width || spirocount == 1) spiroincrement = !spiroincrement;

            if (spiroincrement) {
                if(spirocount >= 4)
                    spirocount *= 2;
                else
                    spirocount += 1;
            }
            else {
                if(spirocount > 4)
                    spirocount /= 2;
                else
                    spirocount -= 1;
            }

            spirooffset = 256 / spirocount;
        }

        if (!change) {
            spirohandledChange = false;
        }
    }

    EVERY_N_MILLIS(33) {
        hue += 1;
    }
}

void SpiralEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("spirocount"))) {
        spirocount = json[F("spirocount")];
    }
}

void SpiralEffect::writeSettings(JsonObject &json)
{
    json[F("spirocount")] = spirocount;
}
