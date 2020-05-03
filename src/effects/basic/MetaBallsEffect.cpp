#include "MetaBallsEffect.h"

MetaBallsEffect::MetaBallsEffect()
{

}

void MetaBallsEffect::tick()
{
    float speed = settings.speed / 127.0;

    // get some 2 random moving points
    uint8_t x2 = inoise8(millis() * speed, 25355, 685 ) / mySettings->matrixSettings.width;
    uint8_t y2 = inoise8(millis() * speed, 355, 11685 ) / mySettings->matrixSettings.height;

    uint8_t x3 = inoise8(millis() * speed, 55355, 6685 ) / mySettings->matrixSettings.width;
    uint8_t y3 = inoise8(millis() * speed, 25355, 22685 ) / mySettings->matrixSettings.height;

    // and one Lissajou function
    uint8_t x1 = beatsin8(23 * speed, 0, 15);
    uint8_t y1 = beatsin8(28 * speed, 0, 15);

    for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
        for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {

            // calculate distances of the 3 points from actual pixel
            // and add them together with weightening
            uint8_t  dx =  abs(x - x1);
            uint8_t  dy =  abs(y - y1);
            uint8_t dist = 2 * sqrt((dx * dx) + (dy * dy));

            dx =  abs(x - x2);
            dy =  abs(y - y2);
            dist += sqrt((dx * dx) + (dy * dy));

            dx =  abs(x - x3);
            dy =  abs(y - y3);
            dist += sqrt((dx * dx) + (dy * dy));

            // inverse result
            //byte color = modes[currentMode].Speed * 10 / dist;
            byte color = 1000 / dist;

            // map color between thresholds
            if (color > 0 and color < 60) {
                if (settings.scale == 100) {
                    myMatrix->drawPixelXY(x, y, CHSV(color * 9, 255, 255));
                } else {
                    myMatrix->drawPixelXY(x,
                                          y,
                                          ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                                                color * 9));
                }
            } else {
                if (settings.scale == 100) {
                    myMatrix->drawPixelXY(x, y, CHSV(0, 255, 255));
                } else {
                    myMatrix->drawPixelXY(x,
                                          y,
                                          ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                                                0));
                }
            }
            // show the 3 points, too
            myMatrix->drawPixelXY(x1, y1, CHSV(255, 255, 255));
            myMatrix->drawPixelXY(x2, y2, CHSV(255, 255, 255));
            myMatrix->drawPixelXY(x3, y3, CHSV(255, 255, 255));
        }
    }
}
