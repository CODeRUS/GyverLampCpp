#include "EffectPulse.h"

namespace {

uint8_t FADE = 0;
uint8_t BLUR = 0;

}

EffectPulse::EffectPulse(const String &id)
    : EffectCalc(id)
{

}

bool EffectPulse::run()
{
    return pulseRoutine();
}

bool EffectPulse::pulseRoutine()
{
#ifdef MIC_EFFECTS
    FADE = 255U - (isMicOn() ? myLamp.getMicMapMaxPeak()*2 : 248U); // (isMicOn() ? 300U - myLamp.getMicMapMaxPeak() : 5U);
    BLUR = (isMicOn() ? myLamp.getMicMapMaxPeak()/3 : 10U); //(isMicOn() ? map(myLamp.getMicMapMaxPeak(), 1, 255, 1, 30) : 10U);
#else
    FADE = 7U;
    BLUR = 10U;
#endif

    CRGBPalette16 palette;
    CRGB _pulse_color;
    uint8_t _pulse_delta = 0;

    palette = RainbowColors_p;
    uint8_t _scale = settings.scale;

    //EffectMath::dimAll(248U); // если эффект устанавливается с другими эффектами от Stefan Petrick, тогда  процедура должна называться dimAll (без двоечки)
    myMatrix->fadeToBlackBy(myMatrix->getNumLeds(), FADE);
    if (pulse_step <= currentRadius) {
        for (uint8_t i = 0; i < pulse_step; i++ ) {
            uint8_t _dark = qmul8( 2U, cos8 (128U / (pulse_step + 1U) * (i + 1U))) ;
            if (_scale == 1) {            // 1 - случайные диски
                _pulse_hue = pulse_hue;
                _pulse_color = CHSV(_pulse_hue, 255U, _dark);

            } else if (_scale <= 17) {    // 2...17 - перелив цвета дисков
                _pulse_delta = (17U - _scale) ;
                _pulse_color = CHSV(_pulse_hueall, 255U, _dark);

            } else if (_scale <= 33) {    // 18...33 - выбор цвета дисков
                _pulse_hue = (_scale - 18U) * 16U ;
                _pulse_color = CHSV(_pulse_hue, 255U, _dark);

            } else if (_scale <= 50) {    // 34...50 - дискоцветы
                _pulse_hue += (_scale - 33U) * 5U ;
                _pulse_color = CHSV(_pulse_hue, 255U, _dark);

            } else if (_scale <= 67) {    // 51...67 - пузыри цветы
                uint8_t _sat =  qsub8( 255U, cos8 (128U / (pulse_step + 1U) * (i + 1U))) ;
                _pulse_hue += (68U - _scale) * 7U ;
                _pulse_color = CHSV(_pulse_hue, _sat, _dark);

            } else if (_scale < 83) {     // 68...83 - выбор цвета пузырей
                uint8_t _sat =  qsub8( 255U, cos8 (128U / (pulse_step + 1U) * (i + 1U))) ;
                _pulse_hue = (_scale - 68U) * 16U ;
                _pulse_color = CHSV(_pulse_hue, _sat, _dark);

            } else if (_scale < 100) {    // 84...99 - перелив цвета пузырей
                uint8_t _sat =  qsub8( 255U, cos8 (128U / (pulse_step + 1U) * (i + 1U))) ;
                _pulse_delta = (_scale - 85U)  ;
                _pulse_color = CHSV(_pulse_hueall, _sat, _dark);

            } else { // 100 - случайные пузыри
                uint8_t _sat =  qsub8( 255U, cos8 (128U / (pulse_step + 1U) * (i + 1U))) ;
                _pulse_hue = pulse_hue;
                _pulse_color = CHSV(_pulse_hue, _sat, _dark);
            }
            EffectMath::drawCircle(centerX, centerY, i, _pulse_color);
        }
    } else {
        centerX = random8(WIDTH - 5U) + 3U;
        centerY = random8(HEIGHT - 5U) + 3U;
        _pulse_hueall += _pulse_delta;
        pulse_hue = random8(0U, 255U);
        currentRadius = random8(3U, 9U);
        pulse_step = 0;
    }
    pulse_step++;
    EffectMath::blur2d(BLUR);
    return true;
}
