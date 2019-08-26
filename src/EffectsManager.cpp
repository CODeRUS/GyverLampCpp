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

namespace  {

std::vector<Effect*> effects;
uint32_t effectTimer = 0;

} // namespace

void EffectsManager::Initialize()
{
    effects = {
        new SparklesEffect(),
        new FireEffect(),
        new VerticalRainbowEffect(),
        new HorizontalRainbowEffect(),
        new ColorsEffect(),
        new MadnessNoiseEffect(),
        new CloudNoiseEffect(),
        new LavaNoiseEffect(),
        new PlasmaNoiseEffect(),
        new RainbowNoiseEffect(),
        new RainbowStripeNoiseEffect(),
        new ZebraNoiseEffect(),
        new ForestNoiseEffect(),
        new OceanNoiseEffect(),
        new ColorEffect(),
        new SnowEffect(),
        new MatrixEffect(),
        new LightersEffect(),
        new ClockEffect(),
        new ClockHorizontal1Effect(),
        new ClockHorizontal2Effect(),
        new ClockHorizontal3Effect(),
        new SoundEffect(),
        new StarfallEffect(),
        new DiagonalRainbowEffect(),
    };
}

void EffectsManager::Process()
{
    if (Settings::currentEffect >= effects.size()) {
        return;
    }

    if (effectTimer != 0 && (millis() - effectTimer) < Settings::CurrentEffectSettings()->effectSpeed) {
        return;
    }
    effectTimer = millis();

    effects[Settings::currentEffect]->Process();
}

void EffectsManager::Next()
{
    effects[Settings::currentEffect]->deactivate();
    myMatrix->clear();
    if (Settings::currentEffect == effects.size() - 1) {
        Settings::currentEffect = 0;
    } else {
        ++Settings::currentEffect;
    }
    ActivateEffect(Settings::currentEffect);
}

void EffectsManager::Previous()
{
    effects[Settings::currentEffect]->deactivate();
    myMatrix->clear();
    if (Settings::currentEffect == 0) {
        Settings::currentEffect = static_cast<uint8_t>(effects.size() - 1);
    } else {
        --Settings::currentEffect;
    }
    ActivateEffect(Settings::currentEffect);
}

void EffectsManager::ChangeEffect(uint8_t index)
{
    if (index >= effects.size()) {
        return;
    }

    if (index == Settings::currentEffect) {
        return;
    }

    effects[Settings::currentEffect]->deactivate();
    myMatrix->clear();
    Settings::currentEffect = index;
    ActivateEffect(index);
    Settings::SaveLater();
}

void EffectsManager::ActivateEffect(uint8_t index)
{
    Effect *effect = effects[index];
    if (!effect->settings) {
        effect->settings = &Settings::effectsSettings[index];
        if (effect->settings->effectScale > 100) {
            effect->settings->effectScale = 50;
        }
    }
    myMatrix->setBrightness(Settings::effectsSettings[index].effectBrightness);
    Serial.printf("%s activated!\n", effect->effectName.c_str());
    Serial.flush();
    effect->activate();
}

uint8_t EffectsManager::Count()
{
    return static_cast<uint8_t>(effects.size());
}

String EffectsManager::EffectName(uint8_t index)
{
    if (index >= effects.size()) {
        return "error";
    }
    return effects[index]->effectName;
}
