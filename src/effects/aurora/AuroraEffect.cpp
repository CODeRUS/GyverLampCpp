#include "AuroraEffect.h"
#include "Boid.h"

AuroraEffect::AuroraEffect()
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
