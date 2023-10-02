#include "chorusLP.h"
#include "delayLP.h"
#include "lfoLP.h"

void ChorusLP::Init(float depth_min_, float mix_, float* buffer_delay, float* buffer_delay_pitch) {
    depth_min = depth_min_;
    mix = mix_;
    lfo.Init(48000, 1.0, 0.01);
    delay.Init(4800, 0.00, 0.0, 1.0, false, buffer_delay, buffer_delay_pitch);
}

void ChorusLP::setDepth(float depth)
{
    lfo.setDepth(depth);
    if (depth > 0.5)
    {
        lfo.setDepth(0.5);
    }
    else if (depth < 0.0)
    {
        lfo.setDepth(0.0);
    }
    else
    {
        lfo.setDepth(depth);
    }
}

void ChorusLP::setSpeed(float speed)
{
    lfo.setSpeed(speed);
}

void ChorusLP::setMix(float new_mix)
{
    mix = new_mix;
}

void ChorusLP::setMinDepth(float new_min)
{
    if (new_min > 0.5)
    {
        new_min = 0.5;
    }
    else if (new_min < 0.0)
    {
        depth_min = 0.0;
    }
    else
    {
        depth_min = new_min;
    }
}

float ChorusLP::getDepth()
{
    return lfo.getDepth();
}

float ChorusLP::getSpeed()
{
    return lfo.getSpeed();
}

float ChorusLP::getMix()
{
    return mix;
}

float ChorusLP::getDepthMin()
{
    return depth_min;
}

void ChorusLP::setFeedback(float fb_new)
{
    delay.setFeedback(fb_new);
}

float ChorusLP::getFeedback()
{
    return delay.getFeedback();
}

float ChorusLP::process(float sample)
{
    delay.setLength((lfo.getFromZero() + depth_min));
    return sample * (1.0 - mix) + delay.process(sample) * mix;
}

