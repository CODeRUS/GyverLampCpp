#include "WhirlEffect.h"
#include "Boid.h"

namespace {

uint16_t ff_x = 0;
uint16_t ff_y = 0;
uint16_t ff_z = 0;

uint8_t hue = 0;

bool oneColor = true;
uint8_t ff_speed = 1;
uint8_t ff_scale = 26;

}

WhirlEffect::WhirlEffect(const String &id)
    : AuroraEffect(id)
{

}

void WhirlEffect::activate()
{
    AuroraEffect::activate();

    ff_x = random16();
    ff_y = random16();
    ff_z = random16();

    for (int i = 0; i < availableBoidCount; i++) {
        boids[i] = Boid(random(mySettings->matrixSettings.width), 0);
    }
}

void WhirlEffect::deactivate()
{
    AuroraEffect::deactivate();
}

void WhirlEffect::tick()
{
    myMatrix->dimAll(240);

    for (int i = 0; i < availableBoidCount; i++) {
        Boid * boid = &boids[i];

        int ioffset = ff_scale * boid->location.x;
        int joffset = ff_scale * boid->location.y;

        byte angle = inoise8(ff_x + ioffset, ff_y + joffset, ff_z);

        boid->velocity.x = sin8(angle) * 0.0078125f - 1.f;
        boid->velocity.y = -(cos8(angle) * 0.0078125f - 1.0f);
        boid->update();

        if (oneColor) {
            myMatrix->drawPixelXY(boid->location.x,
                                  boid->location.y,
                                  CHSV(settings.scale * 2.55, (settings.scale == 100) ? 0 : 255, 255));
        } else {
            myMatrix->drawPixelXY(boid->location.x,
                                  boid->location.y,
                                  ColorFromPalette(*myMatrix->GetColorPalette(settings.scale),
                                                   angle + hue));
        }

        if (boid->location.x < 0
                || boid->location.x >= mySettings->matrixSettings.width
                || boid->location.y < 0
                || boid->location.y >= mySettings->matrixSettings.height) {
            boid->location.x = random(mySettings->matrixSettings.width);
            boid->location.y = 0;
        }
    }

    EVERY_N_MILLIS(200) {
        hue++;
    }

    ff_x += ff_speed;
    ff_y += ff_speed;
    ff_z += ff_speed;
}

void WhirlEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("color"))) {
        oneColor = json[F("color")];
    }
    if (json.containsKey(F("speed"))) {
        ff_speed = json[F("speed")];
    }
    if (json.containsKey(F("scale"))) {
        ff_scale = json[F("scale")];
    }
}

void WhirlEffect::writeSettings(JsonObject &json)
{
    json[F("color")] = oneColor;
    json[F("speed")] = ff_speed;
    json[F("scale")] = ff_scale;
}
