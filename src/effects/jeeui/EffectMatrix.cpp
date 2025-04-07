#include "EffectMatrix.h"

namespace  {

const int LIGHTERS_AM = 100;

int** lightersPos = nullptr;
int8_t** lightersSpeed = nullptr;
CRGB* lightersColor = nullptr;

uint32_t color = CRGB::Green;
bool rnd = false;
uint8_t glitch = 0;

uint8_t hue = 0;

}

EffectMatrix::EffectMatrix(const String &id)
    : EffectCalc(id)
{

}

void EffectMatrix::activate()
{
    lightersPos = new int*[2]();
    lightersPos[0] = new int[LIGHTERS_AM]();
    lightersPos[1] = new int[LIGHTERS_AM]();

    lightersSpeed = new int8_t*[2]();
    lightersSpeed[0] = new int8_t[LIGHTERS_AM]();
    lightersSpeed[1] = new int8_t[LIGHTERS_AM]();

    lightersColor = new CRGB[LIGHTERS_AM]();

    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
        lightersPos[0][i] = random(0, WIDTH);
        lightersPos[1][i] = EffectMath::randomf(HEIGHT - HEIGHT /2, HEIGHT);
        lightersSpeed[0][i] = 1;
        lightersSpeed[1][i] = (float)random(10, 20) / 10.0f;
        lightersColor[i] = color;
    }
}

void EffectMatrix::deactivate()
{
    delete[] lightersPos[0];
    delete[] lightersPos[1];
    delete[] lightersPos;

    delete[] lightersSpeed[0];
    delete[] lightersSpeed[1];
    delete[] lightersSpeed;

    delete[] lightersColor;
}

void EffectMatrix::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("color"))) {
        color = json[F("color")];
    }
    if (json.containsKey(F("glitch"))) {
        glitch = json[F("glitch")];
    }
    if (json.containsKey(F("rnd"))) {
        rnd = json[F("rnd")];
    }
}

void EffectMatrix::writeSettings(JsonObject &json)
{
    json[F("color")] = color;
    json[F("glitch")] = glitch;
    json[F("rnd")] = rnd;
}

bool EffectMatrix::run()
{
    return matrixRoutine();
}

bool EffectMatrix::matrixRoutine()
{
    float speedfactor = (float)settings.speed / 1048.0f + 0.05f;
    EffectMath::dimAll(map(settings.speed, 1, 255, 250, 240));

    for (uint8_t i = 0; i < settings.scale; i++) {
        lightersPos[1][i] -= lightersSpeed[1][i] * speedfactor;

        EffectMath::drawPixelXYF_Y(lightersPos[0][i], lightersPos[1][i], lightersColor[i]);

        if (lightersPos[1][i] <= 0) {
            lightersPos[0][i] = random(0, WIDTH);
            lightersPos[1][i] = EffectMath::randomf(HEIGHT - HEIGHT /2, HEIGHT);
            lightersSpeed[1][i] = EffectMath::randomf(1.5f, 2.5f);

            CRGB curColor = color;
            if (rnd) {
                curColor = CHSV(random8(1, 255), random8(192, 255), random8(192, 255));
            } else if (glitch > 0 && random8() < glitch) {
                lightersPos[0][i] = lightersPos[0][i] + random(-1, 2);
                curColor = CHSV(hue++, 255, random(196, 255));
            }
            lightersColor[i] = curColor;
        }
    }

    return true;
}
