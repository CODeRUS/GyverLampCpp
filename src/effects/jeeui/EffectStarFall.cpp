#include "EffectStarFall.h"

namespace {

const int LIGHTERS_AM = 100;

bool colored = false;
bool isNew = false;

int** lightersPos = nullptr;
int8_t** lightersSpeed = nullptr;
uint8_t* lightersColor = nullptr;
uint8_t* light = nullptr;

}

EffectStarFall::EffectStarFall(const String &id)
    : EffectCalc(id)
{

}

void EffectStarFall::activate()
{
    lightersPos = new int*[2]();
    lightersPos[0] = new int[LIGHTERS_AM]();
    lightersPos[1] = new int[LIGHTERS_AM]();

    lightersSpeed = new int8_t*[2]();
    lightersSpeed[0] = new int8_t[LIGHTERS_AM]();
    lightersSpeed[1] = new int8_t[LIGHTERS_AM]();

    lightersColor = new uint8_t[LIGHTERS_AM]();
    light = new uint8_t[LIGHTERS_AM]();

    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
        lightersPos[0][i] = (float)random(-(WIDTH*10-2), (WIDTH*10-2)) / 10.0f;
        lightersPos[1][i] = random(HEIGHT-4,HEIGHT+4);
        lightersSpeed[0][i] = (float)random(10, 20) / 10.0f;
        lightersSpeed[1][i] = (float)random(10, 20) / 10.0f;
        lightersColor[i] = random(0, 255);
        light[i] = 127;
    }
}

void EffectStarFall::deactivate()
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

void EffectStarFall::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("colored"))) {
        colored = json[F("colored")];
    }
    if (json.containsKey(F("isNew"))) {
        isNew = json[F("isNew")];
    }
}

void EffectStarFall::writeSettings(JsonObject &json)
{
    json[F("colored")] = colored;
    json[F("isNew")] = isNew;
}

bool EffectStarFall::run()
{
    return snowStormStarfallRoutine();
}

bool EffectStarFall::snowStormStarfallRoutine()
{
    float speedfactor = (float)settings.speed / (colored ? 2048.0f : 1024.0f) + 0.15f;
    EffectMath::dimAll(255-map(settings.speed,1,255,25,15));

    for (uint8_t i = 0; i < map(settings.speed,1,255,1,(colored ? LIGHTERS_AM/2 : LIGHTERS_AM)); i++) {
        if (colored) {
            lightersPos[0][i] += lightersSpeed[0][i]*speedfactor;
            if (isNew) {
                lightersPos[1][i] -= lightersSpeed[0][i]*speedfactor;
            } else {
                lightersPos[1][i] -= lightersSpeed[1][i]*speedfactor;
            }
        } else {
            lightersPos[0][i] += lightersSpeed[0][LIGHTERS_AM-1]*speedfactor;
            if (isNew) {
                lightersPos[1][i] -= lightersSpeed[0][LIGHTERS_AM-1]*speedfactor;
            } else {
                lightersPos[1][i] -= lightersSpeed[1][LIGHTERS_AM-1]*speedfactor;
            }
        }
        if (isNew) {
            EffectMath::drawPixelXYF(lightersPos[0][i], lightersPos[1][i], CHSV((colored ? lightersColor[i] : 255), (colored ? light[i] : 0), (colored ? 255 : light[i])));
        } else {
            EffectMath::drawPixelXY((uint8_t)lightersPos[0][i], (uint8_t)lightersPos[1][i], CHSV((colored ? lightersColor[i] : 255), (colored ? light[i] : 0), (colored ? 255 : light[i])));
        }
        if (lightersPos[1][i] <= 0) {
            lightersPos[0][i] = (float)random(-(WIDTH*10-2), (WIDTH*10-2)) / 10.0f;
            lightersPos[1][i] = random(HEIGHT,HEIGHT+4);
            lightersSpeed[0][i] = (float)random(15, 25) / 10.0f;
            lightersSpeed[1][i] = (float)random(15, 25) / 10.0f;
            lightersColor[i] = random(0, 255U);
            light[i] = random(127, 255);
        }
    }
    return true;
}
