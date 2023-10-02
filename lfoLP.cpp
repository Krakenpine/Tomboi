#include "lfoLP.h"

void LfoLP::Init(float samplerate_, float speed_, float depth_)
{
    samplerate = samplerate_;
    speed = speed_;
    depth = depth_;
}

// range -1 to +1
float LfoLP::getNextRaw() {
    index += speed / samplerate;
    if (index > 1.0) {
        index = index - 1.0;
    }
    if (index < 0.25) {
        return index * 4.0;
    } else if (index < 0.50) {
        return (0.50 - index) * 4.0; 
    } else if (index < 0.75) {
        return (index - 0.50) * -4.0;
    } else if (index < 1.0) {
        return (1.0 - index) * -4.0;
    } else {
        return 0.0;
    }
}

void LfoLP::setSpeed(float speed_) {
    if (speed_ < 0.0)
    {
        speed = 0.0;
    }
    else
    {
        speed = speed_;
    }
}

void LfoLP::setDepth(float depth_) {
    if (depth_ > 1.0)
    {
        depth = 1.0;
    }
    else if (depth_ < 0.0)
    {
        depth = 0.0;
    }
    else
    {
        depth = depth_;
    }
}

// oscillating around 1.0
float LfoLP::getTreatedNext()
{
    return 1.0 - (getNextRaw() * depth);
}
float LfoLP::getFromZero()
{
    return (1.0 + getNextRaw()) * 0.5 * depth;
}
float LfoLP::getDepth()
{
    return depth;
}
float LfoLP::getSpeed()
{
    return speed;
}
