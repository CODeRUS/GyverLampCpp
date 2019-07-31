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
