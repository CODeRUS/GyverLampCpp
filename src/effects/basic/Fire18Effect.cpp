#include "Fire18Effect.h"

namespace {

uint32_t* effectX = nullptr;
uint32_t* effectY = nullptr;
uint32_t* effectZ = nullptr;
uint32_t* effectScaleX = nullptr;
uint32_t* effectScaleY = nullptr;
uint8_t*** noise3d = nullptr;

uint8_t** fire18heat = nullptr;

const uint8_t numLayers = 2;
const uint16_t speedArray[2] = {25, 20};

}

Fire18Effect::Fire18Effect(const String &id)
    : Effect(id)
{

}

void Fire18Effect::activate()
{
    effectX = new uint32_t[numLayers];
    effectY = new uint32_t[numLayers];
    effectZ = new uint32_t[numLayers];
    effectScaleX = new uint32_t[numLayers];
    effectScaleY = new uint32_t[numLayers];

    noise3d = new uint8_t**[numLayers]();
    for (uint8_t i = 0; i < numLayers; ++i) {
        noise3d[i] = new uint8_t*[mySettings->matrixSettings.width];
        for (uint8_t j = 0; j < mySettings->matrixSettings.width; ++j) {
            noise3d[i][j] = new uint8_t[mySettings->matrixSettings.height];
        }
    }

    fire18heat = new uint8_t*[mySettings->matrixSettings.width];
    for (uint8_t j = 0; j < mySettings->matrixSettings.width; ++j) {
        fire18heat[j] = new uint8_t[mySettings->matrixSettings.height];
    }
}

void Fire18Effect::deactivate()
{
    delete[] effectX;
    delete[] effectY;
    delete[] effectZ;
    delete[] effectScaleX;
    delete[] effectScaleY;

    for (uint8_t i = 0; i < numLayers; ++i) {
        for (uint8_t j = 0; j < mySettings->matrixSettings.width; j++) {
            delete[] noise3d[i][j];
        }
        delete[] noise3d[i];
    }
    delete[] noise3d;

    for (uint8_t j = 0; j < mySettings->matrixSettings.width; j++) {
        delete[] fire18heat[j];
    }
    delete[] fire18heat;
}

void Fire18Effect::tick()
{
    uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
    uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
    uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);

    for (int z = 0; z < numLayers; z++) {
        const uint16_t speed = speedArray[z];
        effectX[z] = 3 * ctrl * speed;
        effectY[z] = 20 * millis() * speed;
        effectZ[z] = 5 * millis() * speed ;
        effectScaleX[z] = ctrl1 / 2;
        effectScaleY[z] = ctrl2 / 2;

        for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
            uint32_t ioffset = effectScaleX[z] * (i - myMatrix->getCenterX());
            for (uint8_t j = 0; j < mySettings->matrixSettings.height; j++) {
                uint32_t joffset = effectScaleY[z] * (j - myMatrix->getCenterY());
                uint16_t data = ((inoise16(effectX[z] + ioffset, effectY[z] + joffset, effectZ[z])) + 1);
                noise3d[z][i][j] = data >> 8;
            }
        }
    }

    // draw lowest line - seed the fire
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        fire18heat[mySettings->matrixSettings.height - 1][x] =
                noise3d[0][mySettings->matrixSettings.width - 1 - x][myMatrix->getCenterY() - 1];
    }


    //copy everything one line up
    for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
        memcpy(fire18heat[y], fire18heat[y + 1], sizeof(uint8_t) * mySettings->matrixSettings.width);
    }

    //dim
    for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
        for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
            uint8_t dim = noise3d[0][x][y];
            // high value = high flames
            dim = dim / 1.7;
            dim = 255 - dim;
            fire18heat[y][x] = scale8(fire18heat[y][x], dim);
        }
    }

    for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
        for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
            // map the colors based on heatmap
            myMatrix->drawPixelXY(x,
                                  mySettings->matrixSettings.height - 1 - y,
                                  CRGB(fire18heat[y][x],
                                       (float)fire18heat[y][x] * settings.scale * 0.01f,
                                       0));

            // dim the result based on 2nd noise layer
            myMatrix->dimPixelXY(x,
                                  mySettings->matrixSettings.height - 1 - y,
                                  noise3d[1][x][y]);
        }
    }
}
