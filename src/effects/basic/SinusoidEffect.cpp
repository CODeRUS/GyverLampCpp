#include "SinusoidEffect.h"

SinusoidEffect::SinusoidEffect(const String &id)
    : Effect(id)
{

}

void SinusoidEffect::tick()
{
    const uint8_t semiHeightMajor = mySettings->matrixSettings.height / 2 + (mySettings->matrixSettings.height % 2);
    const uint8_t semiWidthMajor =  mySettings->matrixSettings.width / 2  + (mySettings->matrixSettings.width % 2) ;
    float e_s3_speed = 0.004 * settings.speed + 0.015; // speed of the movement along the Lissajous curves
    float e_s3_size = 3 * (float)settings.scale / 100.0 + 2;    // amplitude of the curves

    float time_shift = float(millis() % (uint32_t)(30000 * (1.0 / ((float)settings.speed / 255))));

    for (uint8_t y = 0; y < mySettings->matrixSettings.height; y++) {
        for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
            CRGB color;

            float cx = y + float(e_s3_size * (sinf (float(e_s3_speed * 0.003 * time_shift)))) - semiHeightMajor;  // the 8 centers the middle on a 16x16
            float cy = x + float(e_s3_size * (cosf (float(e_s3_speed * 0.0022 * time_shift)))) - semiWidthMajor;
            float v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
            color.r = v;

            cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0021 * time_shift)))) - semiWidthMajor;
            cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.002 * time_shift)))) - semiHeightMajor;
            v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
            color.b = v;

            cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0041 * time_shift)))) - semiWidthMajor;
            cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.0052 * time_shift)))) - semiHeightMajor;
            v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
            color.g = v;
            myMatrix->drawPixelXY(x, y, color);
        }
    }
}
