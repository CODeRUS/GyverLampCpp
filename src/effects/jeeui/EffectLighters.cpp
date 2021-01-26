#include "EffectLighters.h"

namespace {

const int LIGHTERS_AM = 100;

bool subPix = false;

uint16_t lightersIdx = 0;
float** lightersPos = nullptr;
float** lightersSpeed = nullptr;
uint8_t* lightersColor = nullptr;
uint8_t* light = nullptr;

uint8_t loopCounter = 0;

}

EffectLighters::EffectLighters(const String &id)
    : EffectCalc(id)
{

}

void EffectLighters::activate()
{
    lightersPos = new float*[2]();
    lightersPos[0] = new float[LIGHTERS_AM]();
    lightersPos[1] = new float[LIGHTERS_AM]();

    lightersSpeed = new float*[2]();
    lightersSpeed[0] = new float[LIGHTERS_AM]();
    lightersSpeed[1] = new float[LIGHTERS_AM]();

    lightersColor = new uint8_t[LIGHTERS_AM]();
    light = new uint8_t[LIGHTERS_AM]();

    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
        lightersIdx=0;
        lightersPos[0U][i] = random(0, WIDTH);
        lightersPos[1U][i] = random(0, HEIGHT);
        lightersSpeed[0U][i] = (float)random(-200, 200) / 10.0f;
        lightersSpeed[1U][i] = (float)random(-200, 200) / 10.0f;
        lightersColor[i] = random(0U, 255U);
        light[i] = random(1U, 3U)*127U;
    }
}

void EffectLighters::deactivate()
{
    delete[] lightersPos[0];
    delete[] lightersPos[1];
    delete[] lightersPos;

    delete[] lightersSpeed[0];
    delete[] lightersSpeed[1];
    delete[] lightersSpeed;

    delete[] lightersColor;
    delete[] light;
}

void EffectLighters::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("subPix"))) {
        subPix = json[F("subPix")];
    }
}

void EffectLighters::writeSettings(JsonObject &json)
{
    json[F("subPix")] = subPix;
}

bool EffectLighters::run()
{
    return lightersRoutine();
}

bool EffectLighters::lightersRoutine()
{
    float speedfactor = (float)settings.speed / 4096.0f + 0.001f;

    // EffectMath::blur2d(speed/10);
    //EffectMath::dimAll(50 + speed/10);
    //memset( leds, 0, NUM_LEDS * 3);
    myMatrix->clear();

    uint8_t xscale = map(settings.scale, 0, 100, 1, 32);

//    EVERY_N_MILLIS(333) {
    if (++loopCounter > 20) {
        loopCounter = 0;
        lightersIdx = (lightersIdx+1)%constrain(xscale,1,LIGHTERS_AM);
        lightersSpeed[0U][lightersIdx] += random(-10, 10);
        lightersSpeed[1U][lightersIdx] += random(-10, 10);
        lightersSpeed[0U][lightersIdx] = fmod(lightersSpeed[0U][lightersIdx], 21);
        lightersSpeed[1U][lightersIdx] = fmod(lightersSpeed[1U][lightersIdx], 21);
        light[lightersIdx] = random(255U-(xscale*8),255U);
        if (!random(xscale + 3)) {
            light[lightersIdx] = 127;
        }
    }

    for (uint8_t i = 0U; i < constrain(xscale, 1, LIGHTERS_AM); i++)
    {
        lightersPos[0U][i] += lightersSpeed[0U][i] * speedfactor;
        lightersPos[1U][i] += lightersSpeed[1U][i] * speedfactor;

        if (lightersPos[0U][i] < 0) {
            lightersPos[0U][i] = (float)(WIDTH - 1);
        }
        if (lightersPos[0U][i] >= (float)WIDTH) {
            lightersPos[0U][i] = 0.0f;
        }

        if (lightersPos[1U][i] <= 0.0f) {
            lightersPos[1U][i] = 0.0f;
            lightersSpeed[1U][i] = -lightersSpeed[1U][i];
            lightersSpeed[0U][i] = -lightersSpeed[0U][i];
        }
        if (lightersPos[1U][i] >= (int32_t)(HEIGHT - 1)) {
            lightersPos[1U][i] = (HEIGHT - 1U);
            lightersSpeed[1U][i] = -lightersSpeed[1U][i];
            lightersSpeed[0U][i] = -lightersSpeed[0U][i];
        }

        if (subPix) {
            EffectMath::drawPixelXYF(lightersPos[0U][i], lightersPos[1U][i], CHSV(lightersColor[i], 255U-(i*2), light[i]), 0);
        } else {
            EffectMath::drawPixelXY((uint8_t)lightersPos[0U][i], (uint8_t)lightersPos[1U][i], CHSV(lightersColor[i], 255U-(i*2), light[i]));
        }
    }
    return true;
}
