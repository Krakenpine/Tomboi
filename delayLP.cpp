#include <vector>
#include "delayLP.h"

void DelayLP::Init(int size_, float length_, float fb, float mix_, bool pitch_enabled_, float* buffer_, float* buffer_pitch)
{
    size = size_;
    length = length_;
    feedback = fb;
    mix = mix_;
    pitch_enabled = pitch_enabled_;

    buffer = buffer_;

    pitch_shifter.Init(buffer_pitch);

    index = 0;
    length_samples = static_cast<int>(static_cast<float>(size) * length);
    samplerate = 48000;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
}

void DelayLP::setSamplerate(int samplerate_)
{
    samplerate = samplerate;
}

int DelayLP::getSamplerate()
{
    return samplerate;
}

// get maximum length of delay
int DelayLP::getSize()
{
    return size;
}

// set length as 0.0 - 1.0 from size, set length_samples too
void DelayLP::setLength(float new_length)
{
    if (new_length > 1.0)
    {
        length = 1.0;
    }
    else if (new_length < 0.0)
    {
        length = 0.0;
    }
    else
    {
        length = new_length;
    }
    length_samples = static_cast<int>(static_cast<float>(size) * length);
}

// return delay length as fraction 0.0 - 1.0
float DelayLP::getLength()
{
    return length;
}

// return delay length as samples
int DelayLP::getLengthSamples()
{
    return length_samples;
}

// return delay length
int DelayLP::getLengthMS()
{
    return static_cast<int>((static_cast<float>(length_samples) / static_cast<float>(size)) * 1000.0);
}

// set feedback as fraction 0.0 - 1.0
void DelayLP::setFeedback(float fb_new)
{
    if (fb_new > 1.0)
    {
        feedback = 1.0;
    }
    else if (fb_new < 0.0)
    {
        feedback = 0.0;
    }
    else
    {
        feedback = fb_new;
    }
}

// return feedback as fraction 0.0 - 1.0
float DelayLP::getFeedback()
{
    return feedback;
}

// set mix as faction 0.0 - 1.0
void DelayLP::setMix(float new_mix)
{
    if (new_mix > 1.0)
    {
        mix = 1.0;
    }
    else if (new_mix < 0.0)
    {
        mix = 0.0;
    }
    else
    {
        mix = new_mix;
    }
}

float DelayLP::getMix()
{
    return mix;
}

int DelayLP::getIndex()
{
    return index;
}

void DelayLP::setDelayPitch(float pitch)
{
    pitch_shifter.setAmount(pitch);
}

float DelayLP::process(float sample)
{
    index++;
    if (index >= size)
    {
        index = 0;
    }

    int temp = index - length_samples;
    if (temp < 0)
    {
        temp = size + temp;
    }

    float delayed_signal = buffer[temp];

    float write_temp = sample + delayed_signal * feedback;

    if (pitch_enabled)
    {
        buffer[index] = 0.0 * sample + 0.9 * pitch_shifter.process(write_temp);
    }
    else
    {
        buffer[index] = write_temp;
    }


    return buffer[temp] * mix;
}
