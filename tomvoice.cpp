#include "tomvoice.h"

void TomVoice::Init(Sample sound_)
{
    sound = sound_;
    speed = 1.0;
    sound.setSpeed(1.0);
}

void TomVoice::setSpeed(float speed_)
{
    speed = speed_;
    sound.setSpeed(speed);
}

void TomVoice::start()
{
    sound.startWithExistingSpeed();
}

float TomVoice::getNextSample()
{
    return sound.getNextSample(1.0f);
}