#include "LightersEffect.h"

#define LIGHTERS_AM 100

namespace  {

int lightersPos[2][LIGHTERS_AM];
int8_t lightersSpeed[2][LIGHTERS_AM];
CHSV lightersColor[LIGHTERS_AM];
uint8_t loopCounter;

} // namespace

LightersEffect::LightersEffect()
{
    effectName = "Lighters";

    for (uint8_t i = 0; i < LIGHTERS_AM; i++) {
        lightersPos[0][i] = random(0, width * 10);
        lightersPos[1][i] = random(0, height * 10);
        lightersSpeed[0][i] = random(-10, 10);
        lightersSpeed[1][i] = random(-10, 10);
        lightersColor[i] = CHSV(random(0, 255), 255, 255);
    }
}

void LightersEffect::tick()
{
    myMatrix->clear();
    if (++loopCounter > 20) {
        loopCounter = 0;
    }
    for (uint8_t i = 0; i < settings->effectScale; i++) {
        if (loopCounter == 0) {     // меняем скорость каждые 255 отрисовок
            lightersSpeed[0][i] += random(-3, 4);
            lightersSpeed[1][i] += random(-3, 4);
            lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -20, 20);
            lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -20, 20);
        }

        lightersPos[0][i] += lightersSpeed[0][i];
        lightersPos[1][i] += lightersSpeed[1][i];

        if (lightersPos[0][i] < 0) lightersPos[0][i] = (width - 1) * 10;
        if (lightersPos[0][i] >= width * 10) lightersPos[0][i] = 0;

        if (lightersPos[1][i] < 0) {
            lightersPos[1][i] = 0;
            lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        if (lightersPos[1][i] >= (height - 1) * 10) {
            lightersPos[1][i] = (height - 1) * 10;
            lightersSpeed[1][i] = -lightersSpeed[1][i];
        }
        myMatrix->drawPixelXY(lightersPos[0][i] / 10, lightersPos[1][i] / 10, lightersColor[i]);
    }
}
