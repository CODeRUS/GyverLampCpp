#include "Effect.h"

Effect::Effect()
{
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
    if (json.containsKey(F("id"))) {
        settings.id = json[F("id")].as<String>();
    }
    if (json.containsKey(F("name"))) {
        settings.name = json[F("name")].as<String>();
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
