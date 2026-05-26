#include "EffectWhiteColorStripe.h"


EffectWhiteColorStripe::EffectWhiteColorStripe(const String &id)
    : EffectCalc(id)
{

}

bool EffectWhiteColorStripe::run()
{
    return whiteColorStripeRoutine();
}

bool EffectWhiteColorStripe::whiteColorStripeRoutine()
{
#ifdef MIC_EFFECTS
    byte _scale = isMicOn() ? myLamp.getMicMapMaxPeak() : scale;
#else
    byte _scale = settings.scale * 2.55f;
#endif
    if(_scale <= 127){
        uint8_t centerY = max((uint8_t)round(HEIGHT / 2.0F) - 1, 0);
        uint8_t bottomOffset = (uint8_t)(!(HEIGHT & 1) && (HEIGHT > 1));                      // если высота матрицы чётная, линий с максимальной яркостью две, а линии с минимальной яркостью снизу будут смещены на один ряд
        //Serial.printf_P(PSTR("%d %d\n"), centerY, bottomOffset);
        for (int16_t y = centerY; y >= 0; y--)
        {
            int br = BRIGHTNESS-(13*(WIDTH-y)); if((br-(255-settings.brightness))<0) br=0;

            CRGB color = CHSV(
                        45U,                                                                              // определяем тон
                        map(settings.speed, 0U, 255U, 0U, 170U),                            // определяем насыщенность
                        y == centerY                                                                                                    // определяем яркость
                        ? BRIGHTNESS                                                                                                  // для центральной горизонтальной полосы (или двух) яркость всегда равна BRIGHTNESS
                        : ((_scale) / 143.0F) > ((centerY + 1.0F) - (y + 1.0F)) / (centerY + 1.0F) ? BRIGHTNESS : br);  // для остальных горизонтальных полос яркость равна либо BRIGHTNESS, либо вычисляется по br

            for (int16_t x = 0U; x < (int16_t)WIDTH; x++)
            {
                EffectMath::drawPixelXY(x, y, color);                                                         // при чётной высоте матрицы максимально яркими отрисуются 2 центральных горизонтальных полосы
                EffectMath::drawPixelXY(x, max((uint8_t)(HEIGHT - 1U) - (y + 1U) + bottomOffset, 0U), color); // при нечётной - одна, но дважды
            }
        }
    } else {
        uint8_t centerX = max((uint8_t)round(WIDTH / 2.0F) - 1, 0);
        uint8_t leftOffset = (uint8_t)(!(WIDTH & 1) && (WIDTH > 1));                      // если ширина матрицы чётная, линий с максимальной яркостью две, а линии с минимальной яркостью слева будут смещены на один ряд
        //Serial.printf_P(PSTR("%d %d\n"), centerX, leftOffset);
        for (int16_t y = 0U; y < (int16_t)HEIGHT; y++)
        {
            for (int16_t x = centerX; x >= 0; x--)
            {
                int br = BRIGHTNESS-(13*(WIDTH-x)); if((br-(255-settings.brightness))<0) br=0;

                CRGB color = CHSV(
                            45U,                                                        // определяем тон
                            map(settings.speed, 0U, 255U, 0U, 170U),                            // определяем насыщенность
                            x == centerX                                                                                                    // определяем яркость
                            ? BRIGHTNESS                                                                                                  // для центральной вертикальной полосы (или двух) яркость всегда равна BRIGHTNESS
                            : ((255-_scale) / 143.0F) > ((centerX + 1.0F) - (x + 1.0F)) / (centerX + 1.0F) ? BRIGHTNESS : br);  // для остальных вертикальных полос яркость равна либо BRIGHTNESS, либо вычисляется по br

                EffectMath::drawPixelXY(x, y, color);                                                      // при чётной ширине матрицы максимально яркими отрисуются 2 центральных вертикальных полосы
                EffectMath::drawPixelXY(max((uint8_t)(WIDTH - 1U) - (x + 1U) + leftOffset, 0U), y, color); // при нечётной - одна, но дважды
            }
        }
    }
    return true;
}
