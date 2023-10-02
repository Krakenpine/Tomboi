#include <math.h>
#include <vector>
#include <string>

#include "sample.h"

void Sample::Init(bool loopable_)
{
    loopable = loopable_;

    playing = false;
    speed = 1;
    index = -1;

    frequency = 0;
    midi_note = 0;

    selected_sample = 0;

    fade = false;

}

void Sample::addSample(const float* arr, int size, int loop, float hz, const char* name)
{
    samples.push_back(arr);
    array_sizes.push_back(size);
    loop_points.push_back(loop);
    base_freqs.push_back(hz);
    names.push_back(name);
}

float Sample::getSample(float value)
{
    if (value < 0) {
        return 0;
    }
    float whole = floor(value);
    float decimal = value - whole;
    float lower_sample = samples.at(selected_sample)[int(whole)];            
    float upper_sample;
    if (lower_sample == array_sizes.at(selected_sample) - 1) {
        upper_sample = samples.at(selected_sample)[int(loop_points.at(selected_sample))];
    } else {
        upper_sample = samples.at(selected_sample)[int(whole + 1)];
    }
    return (1.0 - decimal) * lower_sample + decimal * upper_sample;
}

float Sample::frequencyToSpeed()
{
    return frequency / base_freqs.at(selected_sample);
}

float Sample::getNextSample(float speed_mod)
{
    if (!playing)
    {
        return 0;
    }
    if (index < 0)
    {
        index = 0;
    } else
    {
        index += speed * speed_mod;
    }
    if (!loopable) {
        if (index >= array_sizes.at(selected_sample) -1)
        {
            playing = false;
            index = -1;
            return 0;
        }
        else
        {
            return getSample(index);
        }
    } else {
        if (index >= array_sizes.at(selected_sample) -1)
        {
            float index_whole = floor(index);
            float index_decimal = index - index_whole;
            index = loop_points.at(selected_sample) + index_decimal;
        }
        return getSample(index);   
   }
}

float Sample::getHz()
{
    return frequency;
}

bool Sample::isPlaying()
{
    return playing;
}

void Sample::setFrequency(float freq)
{
    frequency = freq;
}

void Sample::setSpeed()
{
    speed = frequencyToSpeed();
}

void Sample::setSpeed(float speed_)
{
    speed = speed_;
}

void Sample::setMidiNote(int midi)
{
    midi_note = midi;
}

void Sample::start()
{
    index = -1;
    playing = true;
    if (samples.size() > 1)
    {
        float wanted_hz = frequency;
        int closest_sample = 0;
        int smallest_difference = 100000;
        for (int i = 0; i < int(array_sizes.size()); i++)
        {
            if ( abs(base_freqs.at(i) - wanted_hz) < smallest_difference )
            {
                smallest_difference = abs(base_freqs.at(i) - wanted_hz);
                closest_sample = i;
            }
        }
        selected_sample = closest_sample;
    }
    else
    {
        selected_sample = 0;
    }
    setSpeed();
}

void Sample::startWithExistingSpeed()
{
    index = -1;
    playing = true;
    selected_sample = 0;
}

void Sample::stop()
{
    playing = false;
    index = -1;
}


float Sample::getSpeed()
{
    return speed;
}
