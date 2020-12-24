#include "ScrollingTextEffect.h"

namespace {

const char defaultText[] PROGMEM = "hello world!";
String text((const __FlashStringHelper*)defaultText);
String line;

uint16_t textColor = myMatrix->Color(40, 40, 40);
uint32_t bgColor = CRGB(0, 0, 0);
bool addSpace = true;

int8_t posx = 0;
uint8_t indexx = 0;

}

ScrollingTextEffect::ScrollingTextEffect(const String &id)
    : Effect(id)
{

}

void ScrollingTextEffect::tick()
{
    myMatrix->fill(bgColor);
    delay(1);

    int16_t x1 = 0;
    int16_t y1 = 0;
    uint16_t w = 0;
    uint16_t h = 0;
    myMatrix->getCharBounds(line.charAt(indexx), &x1, &y1, &w, &h);
    auto posy = (mySettings->matrixSettings.height - h) / 2;

    if (--posx == -w) {
        posx = 0;
        if (++indexx == line.length()) {
            indexx = 0;
        }
    }

    String output = line.substring(indexx);
    if (indexx > 0) {
        output += line.substring(0, indexx);
    }

    myMatrix->setTextColor(textColor);
    myMatrix->setCursor(posx, posy);
    myMatrix->print(output);

    myMatrix->show();
    delay(10);
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
    if (json.containsKey(F("text"))) {
        text = json[F("text")].as<String>();
    }
    if (json.containsKey(F("textColor"))) {
        uint32_t color = json[F("textColor")];
        textColor = myMatrix->Color24to16(color);
    }
    if (json.containsKey(F("bgColor"))) {
        bgColor = json[F("bgColor")];
    }
    if (addSpace) {
        line = text + " ";
    } else {
        line = text;
    }
    delay(50);
}

void ScrollingTextEffect::writeSettings(JsonObject &json)
{
    json[F("text")] = text;
    json[F("textColor")] = myMatrix->expandColor(textColor);
    json[F("bgColor")] = bgColor;
}
