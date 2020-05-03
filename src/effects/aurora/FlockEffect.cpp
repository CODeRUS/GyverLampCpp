#include "FlockEffect.h"
#include "Boid.h"

namespace {

Boid *predator = nullptr;
PVector *wind = nullptr;

uint8_t boidCount = 10;
bool predatorPresent = true;

bool havePredator = false;

uint8_t hue = 0;

}

FlockEffect::FlockEffect()
{

}

void FlockEffect::activate()
{
    AuroraEffect::activate();

    wind = new PVector;

    for (int i = 0; i < boidCount; i++) {
        boids[i] = Boid(15, 15);
        boids[i].maxspeed = 0.380 * settings.speed / 127.0 + 0.380 / 2;
        boids[i].maxforce = 0.015 * settings.speed / 127.0 + 0.015 / 2;
    }
    predatorPresent = havePredator && random(0, 2) >= 1;
    if (predatorPresent) {
        predator = new Boid(31, 31);
        predator->maxspeed = 0.385 * settings.speed / 127.0 + 0.385 / 2;
        predator->maxforce = 0.020 * settings.speed / 127.0 + 0.020 / 2;
        predator->neighbordist = 8.0;
        predator->desiredseparation = 0.0;
    }
}

void FlockEffect::deactivate()
{
    delete wind;
    if (predator) {
        delete predator;
    }

    AuroraEffect::deactivate();
}

void FlockEffect::tick()
{
    myMatrix->applyBlur2d(15);
    myMatrix->dimAll(255 - (settings.scale % 11) * 3);

    bool applyWind = random(0, 255) > 240;
    if (applyWind) {
        wind->x = Boid::randomf() * 0.015f * settings.speed / 127.0f + 0.01f / 2;
        wind->y = Boid::randomf() * 0.015f * settings.speed / 127.0f + 0.015f / 2;
    }

    CRGB color = ColorFromPalette(*myMatrix->GetColorPalette(settings.scale), hue);


    for (int i = 0; i < boidCount; i++) {
        Boid * boid = &boids[i];

        if (predatorPresent) {
            // flee from predator
            boid->repelForce(predator->location, 10);
        }

        boid->run(boids, boidCount);
        boid->wrapAroundBorders();
        PVector location = boid->location;
        // PVector velocity = boid->velocity;
        // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
        // effects.leds[XY(location.x, location.y)] += color;
        myMatrix->drawPixelXY(location.x, location.y, color);

        if (applyWind) {
            boid->applyForce(*wind);
            applyWind = false;
        }
    }

    if (predatorPresent) {
        predator->run(boids, boidCount);
        predator->wrapAroundBorders();
        color = ColorFromPalette(*myMatrix->GetColorPalette(settings.scale), hue + 128);
        PVector location = predator->location;
        // PVector velocity = predator.velocity;
        // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
        // effects.leds[XY(location.x, location.y)] += color;
        myMatrix->drawPixelXY(location.x, location.y, color);
    }

    EVERY_N_MILLIS(333) {
        hue++;
    }

    EVERY_N_SECONDS(30) {
        predatorPresent = havePredator && !predatorPresent;
    }
}

void FlockEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("havePredator"))) {
        havePredator = json[F("havePredator")];
    }
}

void FlockEffect::writeSettings(JsonObject &json)
{
    json[F("havePredator")] = havePredator;
}
