#include "Effect.h"

Effect::Effect()
{
    width = static_cast<uint8_t>(myMatrix->width());
    height = static_cast<uint8_t>(myMatrix->height());
}

Effect::~Effect()
{

}

void Effect::Process() {
    tick();
    myMatrix->show();
}

void Effect::initialize(const JsonObject &json)
{
    update(json);
}

void Effect::update(const JsonObject &json)
{
    if (json.containsKey(F("name"))) {
        String effectName = json[F("name")];
        settings.name = effectName;
    }
    if (json.containsKey(F("speed"))) {
        settings.speed = json[F("speed")];
    }
    if (json.containsKey(F("brightness"))) {
        settings.brightness = json[F("brightness")];
    }
    if (json.containsKey(F("scale"))) {
        settings.scale = json[F("scale")];
    }
}

String Effect::name()
{
    return settings.name;
}

uint8_t Effect::speed()
{
    return settings.speed;
}

uint8_t Effect::scale()
{
    return settings.scale;
}

uint8_t Effect::brightness()
{
   return settings.brightness;
}

void Effect::setBrightness(uint8_t newBrightness)
{
    settings.brightness = newBrightness;
}
