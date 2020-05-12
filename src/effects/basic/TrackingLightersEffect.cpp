#include "TrackingLightersEffect.h"

namespace {

uint8_t amount = 3;
uint8_t clearPath = true;
uint8_t tails = true;
uint8_t tailStep = 70;

int16_t** lightersPos = nullptr;
int8_t** lightersSpeed = nullptr;
CHSV* lightersColor = nullptr;

}

TrackingLightersEffect::TrackingLightersEffect()
{

}

void TrackingLightersEffect::activate()
{
    lightersPos = new int16_t*[2]();
    lightersPos[0] = new int16_t[amount]();
    lightersPos[1] = new int16_t[amount]();

    lightersSpeed = new int8_t*[2]();
    lightersSpeed[0] = new int8_t[amount]();
    lightersSpeed[1] = new int8_t[amount]();

    lightersColor = new CHSV[amount]();

    for (uint8_t j = 0; j < amount; j++) {
        int8_t sign = 0;
        lightersPos[0][j] = mySettings->matrixSettings.width / 2 * 10;
        random(0, 2) ? sign = 1 : sign = -1;
        lightersSpeed[0][j] = random(4, 15) * sign;
        lightersPos[1][j] = mySettings->matrixSettings.height / 2 * 10;
        random(0, 2) ? sign = 1 : sign = -1;
        lightersSpeed[1][j] = random(4, 15) * sign;
        lightersColor[j] = CHSV((settings.scale * (j + 1)) % 256, 255, 255);
    }
}

void TrackingLightersEffect::deactivate()
{
    delete[] lightersPos[0];
    delete[] lightersPos[1];
    delete[] lightersPos;

    delete[] lightersSpeed[0];
    delete[] lightersSpeed[1];
    delete[] lightersSpeed;

    delete[] lightersColor;
}

void TrackingLightersEffect::tick()
{
    if (tails) {
        myMatrix->fader(tailStep);
    } else {
        myMatrix->clear();
    }

    for (uint8_t j = 0; j < amount; j++) {
        for (uint8_t i = 0; i < 2; i++) {
            lightersPos[i][j] += lightersSpeed[i][j];
            if (lightersPos[i][j] < 0) {
                lightersPos[i][j] = 0;
                lightersSpeed[i][j] = -lightersSpeed[i][j];
            }
        }

        if (lightersPos[0][j] > (int16_t)((mySettings->matrixSettings.width - 1) * 10)) {
            lightersPos[0][j] = (mySettings->matrixSettings.width - 1) * 10;
            lightersSpeed[0][j] = -lightersSpeed[0][j];
        }
        if (lightersPos[1][j] > (int16_t)((mySettings->matrixSettings.height - 1) * 10)) {
            lightersPos[1][j] = (mySettings->matrixSettings.height - 1) * 10;
            lightersSpeed[1][j] = -lightersSpeed[1][j];
        }
        myMatrix->drawPixelXY(lightersPos[0][j] / 10, lightersPos[1][j] / 10, lightersColor[j]);
    }
}

void TrackingLightersEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("tails"))) {
        tails = json[F("tails")];
    }
}

void TrackingLightersEffect::writeSettings(JsonObject &json)
{
    json[F("tails")] = tails;
}
