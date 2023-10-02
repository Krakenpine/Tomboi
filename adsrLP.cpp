#include "adsrLP.h"

void ADSR::Init(float samplerate_, float attack_, float decay_, float sustain_, float release_)
{
    samplerate = samplerate_;
    attack = attack_;
    decay = decay_;
    sustain = sustain_;
    release = release_;

    stage = -1;
    counter = 0;
}

void ADSR::setAttack(float attack_)
{
    attack = attack_;
}

void ADSR::setDecay(float decay_)
{
    decay = decay_;
}

void ADSR::setSustain(float sustain_)
{
    sustain = sustain_;
}

void ADSR::setRelease(float release_)
{
    release = release;
}

void ADSR::start()
{
    stage = 1;
    counter = 0;
}

void ADSR::stop()
{
    stage = 4;
}

bool ADSR::isRunning()
{
    return stage != 0;
}

bool ADSR::isInRelease()
{
    return stage == 4;
}

float ADSR::getNext()
{
    if (stage == 1)
    {
        counter += ( 1.0 / attack ) / samplerate;
        if (counter >= 1.0)
        {
            stage = 2;
            counter = 1;
        }
    }
    else if (stage == 2)
    {
        counter -= ((1.0 / decay) / samplerate) * (1.0 - sustain);
        if (counter <= sustain)
        {
            stage = 3;
            counter = sustain;
        }
    }
    else if (stage == 3)
    {
        counter = sustain;
    }
    else if (stage == 4)
    {
        counter -= ((1.0 / release) / samplerate) * sustain;
        if (counter <= 0)
        {
            stage = 0;
        }
    }
    else
    {
        counter = 0;
    }
    return counter;
}
