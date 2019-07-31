#include "Effect.h"

Effect::Effect()
{
    width = MyLED::getWidth();
    height = MyLED::getHeight();
}

Effect::~Effect()
{

}

void Effect::Process() {
    tick();
    MyLED::show();
}
