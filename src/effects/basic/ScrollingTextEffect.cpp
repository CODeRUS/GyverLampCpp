#include "ScrollingTextEffect.h"

namespace {

const char defaultText[] PROGMEM = "hello world!";
String text((const __FlashStringHelper*)defaultText);
String line;

uint32_t textColor = CRGB(40, 40, 40);
uint32_t bgColor = CRGB(0, 0, 0);
bool addSpace = true;

int16_t posx = 0;
uint16_t indexx = 0;

void readColor(const JsonObject &json, const String &key, uint32_t &myColor)
{
    if (json.containsKey(key)) {
        const JsonVariant color = json[key];
        if (color.is<uint32_t>()) {
            myColor = json[key];
        } else if (color.is<JsonObject>()) {
            myColor = json[key]["r"].as<uint8_t>() << 16 |
                      json[key]["g"].as<uint8_t>() << 8 |
                      json[key]["b"].as<uint8_t>();
        }
    }
}

void writeColor(JsonObject &json, const String &key, uint32_t myColor)
{
    JsonObject color = json.createNestedObject(key);
    color["r"] = (uint8_t)(myColor >> 16);
    color["g"] = (uint8_t)(myColor >> 8);
    color["b"] = (uint8_t)(myColor);
}

}

ScrollingTextEffect::ScrollingTextEffect(const String &id)
    : Effect(id)
{

}

void ScrollingTextEffect::tick()
{
    const String currentLine = line;
    const uint16_t lineLength = currentLine.length();
    if (lineLength == 0) {
        return;
    }

    if (indexx >= lineLength) {
        indexx = 0;
        posx = 0;
    }

    myMatrix->fill(bgColor);
    delay(1);

    int16_t x1 = 0;
    int16_t y1 = 0;
    uint16_t w = 0;
    uint16_t h = 0;
    myMatrix->getCharBounds(currentLine.charAt(indexx), &x1, &y1, &w, &h);
    if (w == 0) {
        w = 1;
    }
    auto posy = (mySettings->matrixSettings.height - h) / 2;

    if (--posx <= -static_cast<int16_t>(w)) {
        posx = 0;
        if (++indexx >= lineLength) {
            indexx = 0;
        }
    }

    String output = currentLine.substring(indexx);
    if (indexx > 0) {
        output += currentLine.substring(0, indexx);
    }

    myMatrix->setPassThruColor(textColor);
    myMatrix->setTextColor(0);
    myMatrix->setCursor(posx, posy);
    myMatrix->print(output);

    myMatrix->show();
    delay(10);
    myMatrix->setPassThruColor(0);
}

void ScrollingTextEffect::activate()
{
    myMatrix->setTextWrap(false);

    uint8_t matrixRotation = mySettings->matrixSettings.rotation;
    int horizontalRotation = matrixRotation - 3;
    if (horizontalRotation < 0) {
        horizontalRotation = horizontalRotation + 4;
    }

    if (myMatrix->getRotation() != horizontalRotation) { // for horizontal only
        myMatrix->setRotation(horizontalRotation);
    }
}

void ScrollingTextEffect::deactivate()
{
    uint8_t matrixRotation = myMatrix->getRotation();
    uint8_t settingsRotation = mySettings->matrixSettings.rotation;
    if (matrixRotation != settingsRotation) {
        myMatrix->setRotation(settingsRotation);
    }
}

void ScrollingTextEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    String nextText = text;
    if (json.containsKey(F("text"))) {
        nextText = json[F("text")].as<String>();
    }
    readColor(json, F("textColor"), textColor);
    readColor(json, F("bgColor"), bgColor);
    text = nextText;
    if (addSpace) {
        line = nextText + " ";
    } else {
        line = nextText;
    }
    if (line.length() == 0) {
        line = " ";
    }
    posx = 0;
    indexx = 0;
}

void ScrollingTextEffect::writeSettings(JsonObject &json)
{
    json[F("text")] = text;
    writeColor(json, F("textColor"), textColor);
    writeColor(json, F("bgColor"), bgColor);
}
