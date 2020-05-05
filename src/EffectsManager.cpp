#include "EffectsManager.h"
#include "Settings.h"

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

#include <map>

namespace  {

EffectsManager *instance = nullptr;

std::map<String, Effect*> effectsMap;

uint32_t effectTimer = 0;

uint8_t activeIndex = 0;
bool working = true;

} // namespace

EffectsManager *EffectsManager::Instance()
{
    return instance;
}

void EffectsManager::Initialize()
{
    if (instance) {
        return;
    }

    Serial.println(F("Initializing EffectsManager"));
    instance = new EffectsManager();
}

void EffectsManager::ProcessEffectSettings(const JsonObject &json)
{
    const char* effectId = json[F("id")];

    if (effectsMap.count(effectId) <= 0) {
        Serial.print(F("Missing efect: "));
        Serial.println(effectId);
        return;
    }

    Effect *effect = effectsMap[effectId];
    effects.push_back(effect);
    effect->initialize(json);
}

void EffectsManager::Process()
{
    if (activeIndex >= effects.size()) {
        return;
    }

    if (effectTimer != 0 && (millis() - effectTimer) < activeEffect()->settings.speed) {
        return;
    }
    effectTimer = millis();

    activeEffect()->Process();
}

void EffectsManager::Next()
{
    activeEffect()->deactivate();
    myMatrix->clear();
    if (activeIndex == effects.size() - 1) {
        activeIndex = 0;
    } else {
        ++activeIndex;
    }
    ActivateEffect(activeIndex);
}

void EffectsManager::Previous()
{
    activeEffect()->deactivate();
    myMatrix->clear();
    if (activeIndex == 0) {
        activeIndex = static_cast<uint8_t>(effects.size() - 1);
    } else {
        --activeIndex;
    }
    ActivateEffect(activeIndex);
}

void EffectsManager::ChangeEffect(uint8_t index)
{
    if (index >= effects.size()) {
        return;
    }

    if (index == activeIndex) {
        return;
    }

    activeEffect()->deactivate();
    myMatrix->clear();
    activeIndex = index;
    ActivateEffect(activeIndex);
    mySettings->SaveLater();
}

void EffectsManager::ChangeEffectByName(const String &name)
{
    for (size_t index = 0; index < effects.size(); index++) {
        Effect *effect = effects[index];
        if (effect->settings.name == name) {
            activeEffect()->deactivate();
            myMatrix->clear();
            activeIndex = index;
            ActivateEffect(activeIndex);
            mySettings->SaveLater();

            break;
        }
    }
}

void EffectsManager::ActivateEffect(uint8_t index)
{
    if (index >= effects.size()) {
        index = 0;
    }
    if (activeIndex != index) {
        activeIndex = index;
    }
    Effect *effect = effects[index];
    myMatrix->setBrightness(effect->settings.brightness);
    effect->activate();
}

void EffectsManager::UpdateCurrentSettings(const JsonObject &json)
{
    activeEffect()->initialize(json);
    myMatrix->setBrightness(activeEffect()->settings.brightness);
}

uint8_t EffectsManager::Count()
{
    return static_cast<uint8_t>(effects.size());
}

Effect *EffectsManager::activeEffect()
{
    if (effects.size() > activeIndex) {
        return  effects[activeIndex];
    }

    return nullptr;
}

uint8_t EffectsManager::ActiveEffectIndex()
{
    return activeIndex;
}

EffectsManager::EffectsManager()
{
    randomSeed(micros());

    effectsMap[PSTR("SparklesEffect")] = new SparklesEffect();
    effectsMap[PSTR("FireEffect")] = new FireEffect();
    effectsMap[PSTR("VerticalRainbowEffect")] = new VerticalRainbowEffect();
    effectsMap[PSTR("HorizontalRainbowEffect")] = new HorizontalRainbowEffect();
    effectsMap[PSTR("ColorsEffect")] = new ColorsEffect();
    effectsMap[PSTR("MadnessNoiseEffect")] = new MadnessNoiseEffect();
    effectsMap[PSTR("CloudNoiseEffect")] = new CloudNoiseEffect();
    effectsMap[PSTR("LavaNoiseEffect")] = new LavaNoiseEffect();
    effectsMap[PSTR("PlasmaNoiseEffect")] = new PlasmaNoiseEffect();
    effectsMap[PSTR("RainbowNoiseEffect")] = new RainbowNoiseEffect();
    effectsMap[PSTR("RainbowStripeNoiseEffect")] = new RainbowStripeNoiseEffect();
    effectsMap[PSTR("ZebraNoiseEffect")] = new ZebraNoiseEffect();
    effectsMap[PSTR("ForestNoiseEffect")] = new ForestNoiseEffect();
    effectsMap[PSTR("OceanNoiseEffect")] = new OceanNoiseEffect();
    effectsMap[PSTR("ColorEffect")] = new ColorEffect();
    effectsMap[PSTR("SnowEffect")] = new SnowEffect();
    effectsMap[PSTR("MatrixEffect")] = new MatrixEffect();
    effectsMap[PSTR("LightersEffect")] = new LightersEffect();
    effectsMap[PSTR("ClockEffect")] = new ClockEffect();
    effectsMap[PSTR("ClockHorizontal1Effect")] = new ClockHorizontal1Effect();
    effectsMap[PSTR("ClockHorizontal2Effect")] = new ClockHorizontal2Effect();
    effectsMap[PSTR("ClockHorizontal3Effect")] = new ClockHorizontal3Effect();
    effectsMap[PSTR("SoundEffect")] = new SoundEffect();
    effectsMap[PSTR("StarfallEffect")] = new StarfallEffect();
    effectsMap[PSTR("DiagonalRainbowEffect")] = new DiagonalRainbowEffect();
    effectsMap[PSTR("SoundStereoEffect")] = new SoundStereoEffect();
    effectsMap[PSTR("WaterfallEffect")] = new WaterfallEffect();
    effectsMap[PSTR("TwirlRainbowEffect")] = new TwirlRainbowEffect();
    effectsMap[PSTR("PulseCirclesEffect")] = new PulseCirclesEffect();
    effectsMap[PSTR("AnimationEffect")] = new AnimationEffect();
    effectsMap[PSTR("StormEffect")] = new StormEffect();
    effectsMap[PSTR("Matrix2Effect")] = new Matrix2Effect();
    effectsMap[PSTR("TrackingLightersEffect")] = new TrackingLightersEffect();
    effectsMap[PSTR("LightBallsEffect")] = new LightBallsEffect();
    effectsMap[PSTR("MovingCubeEffect")] = new MovingCubeEffect();
    effectsMap[PSTR("WhiteColorEffect")] = new WhiteColorEffect();
    effectsMap[PSTR("PulsingCometEffect")] = new PulsingCometEffect();
    effectsMap[PSTR("DoubleCometsEffect")] = new DoubleCometsEffect();
    effectsMap[PSTR("TripleCometsEffect")] = new TripleCometsEffect();
    effectsMap[PSTR("RainbowCometEffect")] = new RainbowCometEffect();
    effectsMap[PSTR("ColorCometEffect")] = new ColorCometEffect();
    effectsMap[PSTR("MovingFlameEffect")] = new MovingFlameEffect();
    effectsMap[PSTR("FractorialFireEffect")] = new FractorialFireEffect();
    effectsMap[PSTR("RainbowKiteEffect")] = new RainbowKiteEffect();
    effectsMap[PSTR("BouncingBallsEffect")] = new BouncingBallsEffect();
    effectsMap[PSTR("SpiralEffect")] = new SpiralEffect();
    effectsMap[PSTR("MetaBallsEffect")] = new MetaBallsEffect();
    effectsMap[PSTR("SinusoidEffect")] = new SinusoidEffect();
    effectsMap[PSTR("WaterfallPaletteEffect")] = new WaterfallPaletteEffect();
    effectsMap[PSTR("RainEffect")] = new RainEffect();
    effectsMap[PSTR("PrismataEffect")] = new PrismataEffect();
    effectsMap[PSTR("FlockEffect")] = new FlockEffect();
    effectsMap[PSTR("WhirlEffect")] = new WhirlEffect();
    effectsMap[PSTR("WaveEffect")] = new WaveEffect();
    effectsMap[PSTR("Fire12Effect")] = new Fire12Effect();
    effectsMap[PSTR("Fire18Effect")] = new Fire18Effect();
    effectsMap[PSTR("RainNeoEffect")] = new RainNeoEffect();
}
