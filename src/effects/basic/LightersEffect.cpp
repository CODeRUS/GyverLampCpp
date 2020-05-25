#include "LightersEffect.h"

#define LIGHTERS_AM 100

namespace  {

int** lightersPos = nullptr;
int8_t** lightersSpeed = nullptr;
CHSV* lightersColor = nullptr;

uint8_t loopCounter;

} // namespace

LightersEffect::LightersEffect(const String &id)
    : Effect(id)
{
}

void LightersEffect::activate()
{
    lightersPos = new int*[2]();
    lightersPos[0] = new int[LIGHTERS_AM]();
    lightersPos[1] = new int[LIGHTERS_AM]();

    lightersSpeed = new int8_t*[2]();
    lightersSpeed[0] = new int8_t[LIGHTERS_AM]();
    lightersSpeed[1] = new int8_t[LIGHTERS_AM]();

    lightersColor = new CHSV[LIGHTERS_AM]();

    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
        lightersPos[0][i] = random(0, mySettings->matrixSettings.width * 10);
        lightersPos[1][i] = random(0, mySettings->matrixSettings.height * 10);
        lightersSpeed[0][i] = random(-10, 10);
        lightersSpeed[1][i] = random(-10, 10);
        lightersColor[i] = CHSV(random(0, 255), 255, 255);
    }
}

void LightersEffect::deactivate()
{
    delete[] lightersPos[0];
    delete[] lightersPos[1];
    delete[] lightersPos;

    delete[] lightersSpeed[0];
    delete[] lightersSpeed[1];
    delete[] lightersSpeed;

    delete[] lightersColor;
}

void LightersEffect::tick()
{
    myMatrix->clear();
    if (++loopCounter > 20) {
        loopCounter = 0;
    }
    for (uint8_t i = 0; i < settings.scale; i++) {
        if (loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
            lightersSpeed[0][i] += random(-3, 4);
            lightersSpeed[1][i] += random(-3, 4);
            lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
            lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
        }

        lightersPos[0][i] += lightersSpeed[0][i];
        lightersPos[1][i] += lightersSpeed[1][i];

        if (lightersPos[0][i] < 0) {
            lightersPos[0][i] = (mySettings->matrixSettings.width - 1) * 10;
        }
        if (lightersPos[0][i] >= mySettings->matrixSettings.width * 10) {
            lightersPos[0][i] = 0;
        }

        if (lightersPos[1][i] < 0) {
            lightersPos[1][i] = 0;
            lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        if (lightersPos[1][i] >= (mySettings->matrixSettings.height - 1) * 10) {
            lightersPos[1][i] = (mySettings->matrixSettings.height - 1) * 10;
            lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        myMatrix->drawPixelXY(lightersPos[0][i] / 10, lightersPos[1][i] / 10, lightersColor[i]);
    }
}
