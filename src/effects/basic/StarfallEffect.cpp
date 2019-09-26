#include "StarfallEffect.h"

namespace  {

const uint8_t tailStep = 100;
const uint8_t saturation = 150;

} // namespace

StarfallEffect::StarfallEffect()
{
}

void StarfallEffect::tick()
{
    for (uint8_t i = mySettings->matrixSettings.height / 2; i < mySettings->matrixSettings.height; i++) {
        if (!myMatrix->getPixColorXY(0, i)
                && (random(0, settings.scale) == 0)
                && !myMatrix->getPixColorXY(0, i + 1)
                && !myMatrix->getPixColorXY(0, i - 1))
            myMatrix->setLed(0, i, CHSV(random(0, 200), saturation, 255));
    }

    for (uint8_t i = 0; i < mySettings->matrixSettings.width / 2; i++) {
        if (!myMatrix->getPixColorXY(i, mySettings->matrixSettings.height - 1)
                && (random(0, settings.scale) == 0)
                && !myMatrix->getPixColorXY(i + 1, mySettings->matrixSettings.height - 1)
                && !myMatrix->getPixColorXY(i - 1, mySettings->matrixSettings.height - 1))
            myMatrix->setLed(i, mySettings->matrixSettings.height - 1, CHSV(random(0, 200), saturation, 255));
    }

    // сдвигаем по диагонали
    for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
        for (uint8_t x = mySettings->matrixSettings.width - 1; x > 0; x--) {
            myMatrix->drawPixelXY(x, y, myMatrix->getPixColorXY(x - 1, y + 1));
        }
    }

    // уменьшаем яркость левой и верхней линии, формируем "хвосты"
    for (uint8_t i = mySettings->matrixSettings.height / 2; i < mySettings->matrixSettings.height; i++) {
        myMatrix->fadePixelXY(0, i, tailStep);
    }
    for (uint8_t i = 0; i < mySettings->matrixSettings.width / 2; i++) {
        myMatrix->fadePixelXY(i, mySettings->matrixSettings.height - 1, tailStep);
    }
}
