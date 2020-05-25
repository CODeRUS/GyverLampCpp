#include "Matrix2Effect.h"

Matrix2Effect::Matrix2Effect(const String &id)
    : Effect(id)
{

}

void Matrix2Effect::tick()
{
    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        for (uint8_t y = 0; y < mySettings->matrixSettings.height - 1; y++) {
            CRGB thisColor = myMatrix->getPixColorXY(x, y);
            CRGB upperColor = myMatrix->getPixColorXY(x, y + 1);
            if (myMatrix->colorcode(upperColor) >= 0x900000 && random(7 * mySettings->matrixSettings.height) != 0)
                myMatrix->drawPixelXY(x, y, upperColor);
            else if (!thisColor && random((100 - settings.scale) * mySettings->matrixSettings.height) == 0)
         // else if (!thisColor && random((100 - settings.scale) * mySettings->matrixSettings.height*3) == 0)
                myMatrix->drawPixelXY(x, y, CRGB(0x9bf800));
            else if (myMatrix->colorcode(thisColor) <= 0x050800) {
                if (myMatrix->colorcode(thisColor) >= 0x030000) {
                    myMatrix->drawPixelXY(x, y, CRGB(0x020300));
                } else if (thisColor) {
                    myMatrix->drawPixelXY(x, y, CRGB(0));
                }
            }
            else if (myMatrix->colorcode(thisColor) >= 0x900000) {
                myMatrix->drawPixelXY(x, y, CRGB(0x558800));
            } else {
                myMatrix->drawPixelXY(x, y, thisColor - CRGB(0x0a1000));
             // myMatrix->drawPixelXY(x, y, thisColor - CRGB(0x050800));
            }
        }

        CRGB thisColor = myMatrix->getPixColorXY(x, mySettings->matrixSettings.height - 1);
        if (!thisColor) {
            if (random(100 - settings.scale) == 0) {
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1,
                                      CRGB(0x9bf800));
            }
        }
        else if (myMatrix->colorcode(thisColor) <= 0x050800) {
            if (myMatrix->colorcode(thisColor) >= 0x030000) {
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1,
                                      CRGB(0x020300));
            } else {
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1,
                                      CRGB(0));
            }
        } else if (myMatrix->colorcode(thisColor) >= 0x900000) {
            myMatrix->drawPixelXY(x,
                                  mySettings->matrixSettings.height - 1,
                                  CRGB(0x558800));
        } else {
            myMatrix->drawPixelXY(x,
                                  mySettings->matrixSettings.height - 1,
                                  thisColor - CRGB(0x0a1000));
         // myMatrix->drawPixelXY(x,
         //                       mySettings->matrixSettings.height - 1,
         //                       thisColor - CRGB(0x050800));
        }
    }
}
