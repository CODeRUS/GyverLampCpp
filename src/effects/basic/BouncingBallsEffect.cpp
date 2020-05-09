#include "BouncingBallsEffect.h"

namespace {

bool isColored = false;

float bballsGRAVITY = 9.81f;
uint8_t bballsH0 = 1;
uint8_t bballsMaxNUM = 0;
uint8_t bballsNUM = 0;

float bballsVImpact0 = 0.0f;

uint8_t* bballsCOLOR = nullptr;
uint8_t* bballsX = nullptr;
bool* bballsShift = nullptr;

float* bballsVImpact = nullptr;
int* bballsPos = nullptr;
long* bballsTLast = nullptr;
float* bballsCOR = nullptr;

}

BouncingBallsEffect::BouncingBallsEffect()
{

}

void BouncingBallsEffect::activate()
{
    bballsMaxNUM = mySettings->matrixSettings.width * 2;
    bballsVImpact0 = sqrt(2 * bballsGRAVITY * bballsH0);

    bballsCOLOR = new uint8_t[bballsMaxNUM];
    bballsX = new uint8_t[bballsMaxNUM];
    bballsShift = new bool[bballsMaxNUM];

    bballsVImpact = new float[bballsMaxNUM];
    bballsPos = new int[bballsMaxNUM];
    bballsTLast = new long[bballsMaxNUM];
    bballsCOR = new float[bballsMaxNUM];

    bballsNUM = (settings.scale - 1) / 99.0f * (bballsMaxNUM - 1) + 1;
    if (bballsNUM > bballsMaxNUM) {
        bballsNUM = bballsMaxNUM;
    }
    for (int i = 0 ; i < bballsNUM ; i++) {
        bballsCOLOR[i] = random8();
        bballsX[i] = random8(0, mySettings->matrixSettings.width);
        bballsTLast[i] = millis();
        bballsPos[i] = 0;
        bballsVImpact[i] = bballsVImpact0;
        bballsCOR[i] = 0.90f - float(i) / pow(bballsNUM, 2);
        bballsShift[i] = false;
    }
}

void BouncingBallsEffect::deactivate()
{
    delete[] bballsCOLOR;
    delete[] bballsX;
    delete[] bballsShift;

    delete[] bballsVImpact;
    delete[] bballsPos;
    delete[] bballsTLast;
    delete[] bballsCOR;
}

void BouncingBallsEffect::tick()
{
    float bballsHi = 0;
    float bballsTCycle = 0;
    myMatrix->dimAll(255 - settings.speed);
    for (int i = 0 ; i < bballsNUM ; i++) {
        //leds[XY(bballsX[i], bballsPos[i])] = CRGB::Black;

        bballsTCycle =  millis() - bballsTLast[i];

        bballsHi = -0.5f * bballsGRAVITY * pow(bballsTCycle / 1000.0f , 2.0f) + bballsVImpact[i] * bballsTCycle / 1000.0f;

        if ( bballsHi < 0 ) {
            bballsTLast[i] = millis();
            bballsHi = 0;
            bballsVImpact[i] = bballsCOR[i] * bballsVImpact[i];

            if (bballsVImpact[i] < 0.01f) {
                bballsCOR[i] = 0.90f - float(random(0, 9)) / pow(random(4, 9), 2);
                bballsShift[i] = bballsCOR[i] >= 0.89f;
                bballsVImpact[i] = bballsVImpact0;
            }
        }
        bballsPos[i] = round(bballsHi * (mySettings->matrixSettings.height - 1) / bballsH0);
        if (bballsShift[i] && (bballsPos[i] == mySettings->matrixSettings.height - 1)) {
            bballsShift[i] = false;
            if (bballsCOLOR[i] % 2 == 0) {
                if (bballsX[i] == 0U) bballsX[i] = mySettings->matrixSettings.width - 1;
                else --bballsX[i];
            } else {
                if (bballsX[i] == mySettings->matrixSettings.width - 1) {
                    bballsX[i] = 0;
                } else {
                    ++bballsX[i];
                }
            }
        }
        myMatrix->drawPixelXY(bballsX[i],
                              bballsPos[i],
                              CHSV(bballsCOLOR[i], (isColored) ? 255 : 0, 255));
    }
}

void BouncingBallsEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("colored"))) {
        isColored = json[F("colored")];
    }
}

void BouncingBallsEffect::writeSettings(JsonObject &json)
{
    json[F("colored")] = isColored;
}
