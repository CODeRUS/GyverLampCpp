#include "EffectLighterTracers.h"

namespace {

const uint8_t BALLS_AMOUNT = 7;
float** vector = nullptr;
float** coord = nullptr;
int16_t* ballColors = nullptr;
uint8_t* light = nullptr;

}

EffectLighterTracers::EffectLighterTracers(const String &id)
    : EffectCalc(id)
{

}

bool EffectLighterTracers::run()
{
    return lighterTracersRoutine();
}

void EffectLighterTracers::activate()
{
    vector = new float*[2]();
    vector[0] = new float[BALLS_AMOUNT]();
    vector[1] = new float[BALLS_AMOUNT]();

    coord = new float*[2]();
    coord[0] = new float[BALLS_AMOUNT]();
    coord[1] = new float[BALLS_AMOUNT]();

    ballColors = new int16_t[BALLS_AMOUNT]();
    light = new uint8_t[BALLS_AMOUNT]();

    for (uint8_t j = 0; j < BALLS_AMOUNT; j++)
    {
        int8_t sign;
        // забиваем случайными данными
        coord[0][j] = (float)WIDTH / 2.0f;
        random(0, 2) ? sign = 1 : sign = -1;
        vector[0][j] = ((float)random(40, 150) / 10.0f) * sign;
        coord[1][j] = (float)HEIGHT / 2;
        random(0, 2) ? sign = 1 : sign = -1;
        vector[1][j] = ((float)random(40, 150) / 10.0f) * sign;
        light[j] = 127;
        //ballColors[j] = random(0, 9) * 28;
    }
}

void EffectLighterTracers::deactivate()
{
    delete[] vector[0];
    delete[] vector[1];
    delete[] vector;

    delete[] coord[0];
    delete[] coord[1];
    delete[] coord;

    delete[] ballColors;
    delete[] light;
}

bool EffectLighterTracers::lighterTracersRoutine()
{
    uint8_t xscale = map(settings.scale, 0, 100, 1, 255);
    //float speedfactor = speed/2048.0+0.01;
    float speedfactor = (float)xscale / 4096.0f + 0.001f;

    myMatrix->fadeToBlackBy(255 - (uint8_t)(10 * ((float)settings.speed) /255) + 40);

    // движение шариков
    int maxBalls = (uint8_t)((BALLS_AMOUNT/255.0)*xscale+0.99);
    for (uint8_t j = 0U; j < maxBalls; j++)
    {
        // цвет зависит от масштаба
        ballColors[j] = xscale * (maxBalls-j) * BALLS_AMOUNT + j;

        // движение шариков
        for (uint8_t i = 0U; i < 2U; i++)
        {
            coord[i][j] += vector[i][j] * speedfactor;
            if (coord[i][j] < 0)
            {
                coord[i][j] = 0.0f;
                vector[i][j] = -vector[i][j];
            }
        }

        if ((uint16_t)coord[0][j] > (WIDTH - 1))
        {
            coord[0][j] = (WIDTH - 1);
            vector[0][j] = -vector[0][j];
        }
        if ((uint16_t)coord[1][j] > (HEIGHT - 1))
        {
            coord[1][j] = (HEIGHT - 1);
            vector[1][j] = -vector[1][j];
        }
        EVERY_N_MILLIS(random16(1024)) {
            if (light[j] == 127)
                light[j] = 255;
            else light[j] = 127;
        }
        EffectMath::drawPixelXYF(coord[0][j], coord[1][j], CHSV(ballColors[j], 200U, 255U));
    }
    return true;
}
