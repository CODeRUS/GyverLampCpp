#include "Effect.h"

Effect::Effect(const String &id)
{
    settings.id = id;
    settings.name = id;
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
    if (json.containsKey(F("i"))) {
        settings.id = json[F("i")].as<String>();
    }
    if (json.containsKey(F("n"))) {
        settings.name = json[F("n")].as<String>();
    }
    if (json.containsKey(F("name"))) {
        settings.name = json[F("name")].as<String>();
    }
    if (json.containsKey(F("s"))) {
        settings.speed = json[F("s")];
    }
    if (json.containsKey(F("speed"))) {
        settings.speed = json[F("speed")];
    }
    if (json.containsKey(F("b"))) {
        settings.brightness = json[F("b")];
    }
    if (json.containsKey(F("brightness"))) {
        settings.brightness = json[F("brightness")];
    }
    if (json.containsKey(F("l"))) {
        settings.scale = json[F("l")];
    }
    if (json.containsKey(F("scale"))) {
        settings.scale = json[F("scale")];
    }
}

void Effect::writeSettings(JsonObject &json)
{

}
