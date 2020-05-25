#include "AuroraEffect.h"
#include "Boid.h"

AuroraEffect::AuroraEffect(const String &id)
    : Effect(id)
{

}

void AuroraEffect::activate()
{
    boids = new Boid[availableBoidCount];
}

void AuroraEffect::deactivate()
{
    delete[] boids;
}
