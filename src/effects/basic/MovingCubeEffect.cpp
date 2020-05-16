#include "MovingCubeEffect.h"

namespace {

bool randomColor = true;

int16_t* coordB = nullptr;
int8_t* vectorB = nullptr;
CRGB ballColor = CRGB::Black;
int8_t ballSize = 0;

}

MovingCubeEffect::MovingCubeEffect()
{

}

void MovingCubeEffect::tick()
{
    ballSize = map(settings.scale * 2.55, 0, 255, 2,
                   max(min(mySettings->matrixSettings.width,
                           mySettings->matrixSettings.height) / 3, 2));

    for (uint8_t i = 0; i < 2; i++) {
        coordB[i] += vectorB[i];
        if (coordB[i] < 0) {
            coordB[i] = 0;
            vectorB[i] = -vectorB[i];
            if (randomColor) {
                ballColor = CHSV(random(0, 9) * 28, 255, 255);
            }
            //vectorB[i] += random(0, 6) - 3;
        }
    }
    if (coordB[0] > (int16_t)((mySettings->matrixSettings.width - ballSize) * 10)) {
        coordB[0] = (mySettings->matrixSettings.width - ballSize) * 10;
        vectorB[0] = -vectorB[0];
        if (randomColor) {
            ballColor = CHSV(random(0, 9) * 28, 255, 255);
        }
        //vectorB[0] += random(0, 6) - 3;
    }
    if (coordB[1] > (int16_t)((mySettings->matrixSettings.height - ballSize) * 10)) {
        coordB[1] = (mySettings->matrixSettings.height - ballSize) * 10;
        vectorB[1] = -vectorB[1];
        if (randomColor) {
            ballColor = CHSV(random(0, 9) * 28, 255, 255);
        }
        //vectorB[1] += random(0, 6) - 3;
    }
    myMatrix->clear();
    for (uint8_t i = 0; i < ballSize; i++) {
        for (uint8_t j = 0; j < ballSize; j++) {
            myMatrix->drawPixelXY(coordB[0] / 10 + i,
                                  coordB[1] / 10 + j,
                                  ballColor);
        }
    }
}

void MovingCubeEffect::activate()
{
    coordB = new int16_t[2]();
    vectorB = new int8_t[2]();

    for (uint8_t i = 0; i < 2; i++) {
        coordB[i] = mySettings->matrixSettings.width / 2 * 10;
        vectorB[i] = random(8, 20);
    }

    ballColor = CHSV(random(0, 9) * 28, 255, 255);
}

void MovingCubeEffect::deactivate()
{
    delete[] coordB;
    delete[] vectorB;
}

void MovingCubeEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("rnd"))) {
        randomColor = json[F("rnd")];
    }
}

void MovingCubeEffect::writeSettings(JsonObject &json)
{
    json[F("rnd")] = randomColor;
}
