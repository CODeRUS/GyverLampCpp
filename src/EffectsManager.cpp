#include "EffectsManager.h"
#include "Settings.h"
#include "MqttClient.h"
#include "LampWebServer.h"

#include "effects/basic/SparklesEffect.h"
#include "effects/basic/FireEffect.h"
#include "effects/basic/MatrixEffect.h"
#include "effects/basic/VerticalRainbowEffect.h"
#include "effects/basic/HorizontalRainbowEffect.h"
#include "effects/basic/ColorEffect.h"
#include "effects/basic/ColorsEffect.h"
#include "effects/basic/SnowEffect.h"
#include "effects/basic/LightersEffect.h"
#include "effects/basic/ClockEffect.h"
#include "effects/basic/ClockHorizontal1Effect.h"
#include "effects/basic/ClockHorizontal2Effect.h"
#include "effects/basic/ClockHorizontal3Effect.h"
#include "effects/basic/StarfallEffect.h"
#include "effects/basic/DiagonalRainbowEffect.h"

#include "effects/noise/MadnessNoiseEffect.h"
#include "effects/noise/CloudNoiseEffect.h"
#include "effects/noise/LavaNoiseEffect.h"
#include "effects/noise/PlasmaNoiseEffect.h"
#include "effects/noise/RainbowNoiseEffect.h"
#include "effects/noise/RainbowStripeNoiseEffect.h"
#include "effects/noise/ZebraNoiseEffect.h"
#include "effects/noise/ForestNoiseEffect.h"
#include "effects/noise/OceanNoiseEffect.h"

#include "effects/sound/SoundEffect.h"
#include "effects/sound/SoundStereoEffect.h"

#include "effects/basic/WaterfallEffect.h"
#include "effects/basic/TwirlRainbowEffect.h"
#include "effects/basic/PulseCirclesEffect.h"
#include "effects/basic/AnimationEffect.h"
#include "effects/basic/StormEffect.h"
#include "effects/basic/Matrix2Effect.h"
#include "effects/basic/TrackingLightersEffect.h"
#include "effects/basic/LightBallsEffect.h"
#include "effects/basic/MovingCubeEffect.h"
#include "effects/basic/WhiteColorEffect.h"

#include "effects/fractional/PulsingCometEffect.h"
#include "effects/fractional/DoubleCometsEffect.h"
#include "effects/fractional/TripleCometsEffect.h"
#include "effects/fractional/RainbowCometEffect.h"
#include "effects/fractional/ColorCometEffect.h"
#include "effects/fractional/MovingFlameEffect.h"
#include "effects/fractional/FractorialFireEffect.h"
#include "effects/fractional/RainbowKiteEffect.h"

#include "effects/basic/BouncingBallsEffect.h"
#include "effects/basic/SpiralEffect.h"
#include "effects/basic/MetaBallsEffect.h"
#include "effects/basic/SinusoidEffect.h"
#include "effects/basic/WaterfallPaletteEffect.h"
#include "effects/basic/RainEffect.h"
#include "effects/basic/PrismataEffect.h"

#include "effects/aurora/FlockEffect.h"
#include "effects/aurora/WhirlEffect.h"
#include "effects/aurora/WaveEffect.h"

#include "effects/basic/Fire12Effect.h"
#include "effects/basic/Fire18Effect.h"
#include "effects/basic/RainNeoEffect.h"
#include "effects/basic/TwinklesEffect.h"

#include "effects/network/DMXEffect.h"

#include "effects/basic/ScrollingTextEffect.h"

#include "effects/jeeui/EffectPulse.h"
#include "effects/jeeui/EffectSparcles.h"
#include "effects/jeeui/EffectWhiteColorStripe.h"
#include "effects/jeeui/EffectEverythingFall.h"
#include "effects/jeeui/EffectMatrix.h"
#include "effects/jeeui/EffectStarFall.h"
#include "effects/jeeui/EffectLighters.h"
#include "effects/jeeui/EffectLighterTracers.h"
#include "effects/jeeui/EffectLightBalls.h"
#include "effects/jeeui/EffectBall.h"

#include <map>

namespace  {

EffectsManager *object = nullptr;

std::map<String, Effect*> effectsMap;

uint32_t effectTimer = 0;

uint8_t activeIndex = 0;

} // namespace

EffectsManager *EffectsManager::instance()
{
    return object;
}

void EffectsManager::Initialize()
{
    if (object) {
        return;
    }

    Serial.println(F("Initializing EffectsManager"));
    object = new EffectsManager();
}

void EffectsManager::processEffectSettings(const JsonObject &json)
{
    const String effectId = json[F("i")].as<String>();

    if (effectsMap.count(effectId) <= 0) {
        Serial.print(F("Missing effect: "));
        Serial.println(effectId);
        return;
    }

    Effect *effect = effectsMap[effectId];
    effects.push_back(effect);
    effect->initialize(json);
}

void EffectsManager::processAllEffects()
{
    for (auto effectPair : effectsMap) {
        effects.push_back(effectPair.second);
    }
}

void EffectsManager::loop()
{
    if (activeIndex >= effects.size()) {
        return;
    }

    if (effectTimer != 0 && (millis() - effectTimer) < (255 - activeEffect()->settings.speed)) {
        if (mySettings->matrixSettings.dither) {
            FastLED.show();
        }
        return;
    }
    effectTimer = millis();

    activeEffect()->Process();
}

void EffectsManager::next()
{
    myMatrix->clear();
    uint8_t aIndex = activeIndex;
    if (aIndex == effects.size() - 1) {
        aIndex = 0;
    } else {
        ++aIndex;
    }
    activateEffect(aIndex);
}

void EffectsManager::previous()
{
    myMatrix->clear();
    uint8_t aIndex = activeIndex;
    if (aIndex == 0) {
        aIndex = static_cast<uint8_t>(effects.size() - 1);
    } else {
        --aIndex;
    }
    activateEffect(aIndex);
}

void EffectsManager::changeEffectByName(const String &name)
{
    for (size_t index = 0; index < effects.size(); index++) {
        Effect *effect = effects[index];
        if (effect->settings.name == name) {
            activateEffect(index);
            break;
        }
    }
}

void EffectsManager::changeEffectById(const String &id)
{
    for (size_t index = 0; index < effects.size(); index++) {
        Effect *effect = effects[index];
        if (effect->settings.id == id) {
            activateEffect(index);
            break;
        }
    }
}

void EffectsManager::activateEffect(uint8_t index, bool save)
{
    if (index >= effects.size()) {
        index = 0;
    }
    myMatrix->clear();
    activeEffect()->deactivate();
    if (activeIndex != index) {
        activeIndex = index;
    }
    Effect *effect = effects[index];
    myMatrix->setBrightness(effect->settings.brightness);
    Serial.printf_P(PSTR("Activating effect[%u]: %s\n"), index, effect->settings.name.c_str());
    effect->activate();
    if (mqtt) {
        mqtt->update();
    }
    lampWebServer->update();
    if (!save) {
        return;
    }
    mySettings->saveLater();
}

void EffectsManager::updateCurrentSettings(const JsonObject &json)
{
    activeEffect()->initialize(json);
    myMatrix->setBrightness(activeEffect()->settings.brightness);
    mySettings->saveLater();
}

void EffectsManager::updateSettingsById(const String &id, const JsonObject &json)
{
    for (size_t index = 0; index < effects.size(); index++) {
        Effect *effect = effects[index];
        if (effect->settings.id == id) {
            if (effect != effects[activeIndex]) {
                activateEffect(index);
            }
            effect->initialize(json);
            break;
        }
    }
    myMatrix->setBrightness(activeEffect()->settings.brightness);
    mySettings->saveLater();
}

uint8_t EffectsManager::count()
{
    return static_cast<uint8_t>(effects.size());
}

Effect *EffectsManager::activeEffect()
{
    if (effects.size() > activeIndex) {
        return effects[activeIndex];
    }

    return nullptr;
}

uint8_t EffectsManager::activeEffectIndex()
{
    return activeIndex;
}

template <typename T>
void RegisterEffect(const String &id)
{
    effectsMap[id] = new T(id);
}

EffectsManager::EffectsManager()
{
    randomSeed(micros());

    RegisterEffect<SparklesEffect>(F("Sparkles"));
    RegisterEffect<FireEffect>(F("Fire"));
    RegisterEffect<VerticalRainbowEffect>(F("VerticalRainbow"));
    RegisterEffect<HorizontalRainbowEffect>(F("HorizontalRainbow"));
    RegisterEffect<ColorsEffect>(F("Colors"));
    RegisterEffect<MadnessNoiseEffect>(F("MadnessNoise"));
    RegisterEffect<CloudNoiseEffect>(F("CloudNoise"));
    RegisterEffect<LavaNoiseEffect>(F("LavaNoise"));
    RegisterEffect<PlasmaNoiseEffect>(F("PlasmaNoise"));
    RegisterEffect<RainbowNoiseEffect>(F("RainbowNoise"));
    RegisterEffect<RainbowStripeNoiseEffect>(F("RainbowStripeNoise"));
    RegisterEffect<ZebraNoiseEffect>(F("ZebraNoise"));
    RegisterEffect<ForestNoiseEffect>(F("ForestNoise"));
    RegisterEffect<OceanNoiseEffect>(F("OceanNoise"));
    RegisterEffect<ColorEffect>(F("Color"));
    RegisterEffect<SnowEffect>(F("Snow"));
    RegisterEffect<MatrixEffect>(F("Matrix"));
    RegisterEffect<LightersEffect>(F("Lighters"));
    RegisterEffect<ClockEffect>(F("Clock"));
    RegisterEffect<ClockHorizontal1Effect>(F("ClockHorizontal1"));
    RegisterEffect<ClockHorizontal2Effect>(F("ClockHorizontal2"));
    RegisterEffect<ClockHorizontal3Effect>(F("ClockHorizontal3"));
    RegisterEffect<StarfallEffect>(F("Starfall"));
    RegisterEffect<DiagonalRainbowEffect>(F("DiagonalRainbow"));
    RegisterEffect<WaterfallEffect>(F("Waterfall"));
    RegisterEffect<TwirlRainbowEffect>(F("TwirlRainbow"));
    RegisterEffect<PulseCirclesEffect>(F("PulseCircles"));
    RegisterEffect<AnimationEffect>(F("Animation"));
    RegisterEffect<StormEffect>(F("Storm"));
    RegisterEffect<Matrix2Effect>(F("Matrix2"));
    RegisterEffect<TrackingLightersEffect>(F("TrackingLighters"));
    RegisterEffect<LightBallsEffect>(F("LightBalls"));
    RegisterEffect<MovingCubeEffect>(F("MovingCube"));
    RegisterEffect<WhiteColorEffect>(F("WhiteColor"));
    RegisterEffect<PulsingCometEffect>(F("PulsingComet"));
    RegisterEffect<DoubleCometsEffect>(F("DoubleComets"));
    RegisterEffect<TripleCometsEffect>(F("TripleComets"));
    RegisterEffect<RainbowCometEffect>(F("RainbowComet"));
    RegisterEffect<ColorCometEffect>(F("ColorComet"));
    RegisterEffect<MovingFlameEffect>(F("MovingFlame"));
    RegisterEffect<FractorialFireEffect>(F("FractorialFire"));
    RegisterEffect<RainbowKiteEffect>(F("RainbowKite"));
    RegisterEffect<BouncingBallsEffect>(F("BouncingBalls"));
    RegisterEffect<SpiralEffect>(F("Spiral"));
    RegisterEffect<MetaBallsEffect>(F("MetaBalls"));
    RegisterEffect<SinusoidEffect>(F("Sinusoid"));
    RegisterEffect<WaterfallPaletteEffect>(F("WaterfallPalette"));
    RegisterEffect<RainEffect>(F("Rain"));
    RegisterEffect<PrismataEffect>(F("Prismata"));
    RegisterEffect<FlockEffect>(F("Flock"));
    RegisterEffect<WhirlEffect>(F("Whirl"));
    RegisterEffect<WaveEffect>(F("Wave"));
    RegisterEffect<Fire12Effect>(F("Fire12"));
    RegisterEffect<Fire18Effect>(F("Fire18"));
    RegisterEffect<RainNeoEffect>(F("RainNeo"));
    RegisterEffect<TwinklesEffect>(F("Twinkles"));
//    RegisterEffect<SoundEffect>(F("Sound"));
//    RegisterEffect<SoundStereoEffect>(F("Stereo"));
    RegisterEffect<DMXEffect>(F("DMX"));
    RegisterEffect<ScrollingTextEffect>(F("Text"));

    RegisterEffect<EffectPulse>(F("PulseJ"));
    RegisterEffect<EffectSparcles>(F("SparclesJ"));
    RegisterEffect<EffectWhiteColorStripe>(F("WhiteJ"));
    RegisterEffect<EffectEverythingFall>(F("FallJ"));
    RegisterEffect<EffectMatrix>(F("MatrixJ"));
    RegisterEffect<EffectStarFall>(F("StarfallJ"));
    RegisterEffect<EffectLighters>(F("LightersJ"));
    RegisterEffect<EffectLighterTracers>(F("LighterTracersJ"));
    RegisterEffect<EffectLightBalls>(F("LightBallsJ"));
    RegisterEffect<EffectBall>(F("BallJ"));
}
