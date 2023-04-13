#include "EffectSparcles.h"

namespace {

String micMode10 = "1";
uint8_t micScale = 1;
uint8_t amount = 1;

}

EffectSparcles::EffectSparcles(const String &id)
    : EffectCalc(id)
{

}

bool EffectSparcles::run()
{
    return sparklesRoutine();
}

String EffectSparcles::getCtrlVal(uint8_t idx)
{
    switch (idx) {
    case 1:
        return String(amount);
    case 2:
        return String(micScale);
    case 3:
        return micMode10;
    default:
        return String();
    }
}

void EffectSparcles::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("micMode10"))) {
        micMode10 = json[F("micMode10")].as<String>();
    }
    if (json.containsKey(F("micScale"))) {
        micScale = json[F("micScale")];
    }
    if (json.containsKey(F("amount"))) {
        amount = json[F("amount")];
    }
}

void EffectSparcles::writeSettings(JsonObject &json)
{
    json[F("micMode10")] = micMode10;
    json[F("micScale")] = micScale;
    json[F("amount")] = amount;
}

bool EffectSparcles::sparklesRoutine()
{

#ifdef MIC_EFFECTS
    uint8_t mic = myLamp.getMicMapMaxPeak();
    uint8_t mic_f = map(myLamp.getMicMapFreq(), LOW_FREQ_MAP_VAL, HI_FREQ_MAP_VAL, 0, 255);
    if (isMicOn() and getCtrlVal(3).toInt() > 5)
        fadeToBlackBy(leds, NUM_LEDS, 255 - mic);
    EffectMath::fader(isMicOn() ? map(getCtrlVal(2).toInt(), 1, 255, 100, 1) : map(getCtrlVal(2).toInt(), 1, 255, 50, 1));
#else
    EffectMath::fader(map(settings.scale, 1, 100, 1, 50));
#endif

    CHSV currentHSV;

    for (uint8_t i = 0; i < (uint8_t)round(2.5 * (getCtrlVal(1).toInt() / 255.0) + 1); i++) {
        uint8_t x = random8(0U, WIDTH);
        uint8_t y = random8(0U, HEIGHT);
#ifdef MIC_EFFECTS
        switch (getCtrlVal(3).toInt())
        {
        case 1 :
        case 6 :
            mic_f = myLamp.getMicMapFreq();
            break;
        case 2 :
        case 7 :
            mic_f = map(myLamp.getMicMapFreq(), LOW_FREQ_MAP_VAL, HI_FREQ_MAP_VAL, 0, 255);
            break;
        case 3 :
        case 8 :
            mic_f = sin8(myLamp.getMicMapFreq());
            break;
        case 4 :
        case 9 :
            mic_f = cos8(myLamp.getMicMapFreq());
            break;
        case 5 :
        case 10 :
            mic_f = random8(255U);
            break;
        }

#endif
        if (EffectMath::getPixColorXY(x, y) == 0U) {
#ifdef MIC_EFFECTS
            if (isMicOn()) {
                currentHSV = CHSV(mic_f, 255U - myLamp.getMicMapMaxPeak()/3, constrain(mic * 1.25f, 48, 255));
            }
            else
                currentHSV = CHSV(random8(1U, 255U), random8(192U, 255U), random8(192U, 255U));
#else
            currentHSV = CHSV(random8(1U, 255U), random8(192U, 255U), random8(192U, 255U));
#endif
            EffectMath::drawPixelXY(x, y, currentHSV);
        }
    }
    return true;
}
