#include <cmath>

#include "daisysp.h"
#include "pitchLP.h"

using namespace daisysp;


void PitchLP::Init(float* buffer_)
{
    buffer = buffer_;
    buf_size = 48000;
    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
        buffer[i + buf_size] = 0;
        buffer[i + buf_size * 2] = 0;
        buffer[i + buf_size * 3] = 0;
    }
    index_write = 0;
    buffer_select = true;
    sample_raw_prev = 0;
    amount = 1;
    filt1.Init(48000);
    filt1.SetRes(0.1f);
    filt1.SetDrive(0.1f);
    filt1.SetFreq(100.0f);
}

void PitchLP::setAmount(float amount_)
{
    amount = amount_;
}

float PitchLP::tanhWindower(int index)
{
    float index_norm = float(index) / float(size);
    if (amount < 1.0)
    {
        index_norm = index_norm / amount;
    }
    if (index_norm < 0.5)
    {
        index_norm = (index_norm * 18.0) - 2.0;
    }
    else
    {
        index_norm = ((1.0 - index_norm) * 18.0) - 2.0;
    }
    float temp = 0;
    if (index_norm < -3.f)
    {
        temp = -1.f;
    }
    else if (index_norm > 3.f)
    {
        temp = 1.f;
    }
    else
    {
        float in2 = index_norm*index_norm;
        temp = index_norm * (27.f + in2) / (27.f + in2 * 9.f);
    }
    return (temp + 1.0) / 2.0;
}

float PitchLP::windower(int index)
{
    float index_norm = float(index) / float(size);
    if (amount < 1.0)
    {
        index_norm = index_norm / amount;
    }
    if (index_norm < 0.5)
    {
        return index_norm * 2.0;
    }
    else if (index_norm > 0.5)
    {
        return (1.0 - index_norm) * 2.0;
    }
    else
    {
        return 1.0;
    }
}

float PitchLP::process(float sample_raw)
{
    filt1.Process(sample_raw);
    float sample = filt1.High();

    float output = 0;
    index_write++;

    if (index_write >= size)
    {
        buffer_select = !buffer_select;
        index_write = 0;
    }

    int index_write1 = index_write;
    int index_write2 = (index_write + (size / 4)) % size;
    int index_write3 = (index_write + (size / 2)) % size;
    int index_write4 = (index_write + ((3 * size) / 4)) % size;

    buffer[index_write1] = sample;
    buffer[index_write2 + buf_size] = sample;
    buffer[index_write3 + buf_size * 2] = sample;
    buffer[index_write4 + buf_size * 3] = sample;

    float kerroin = amount;
    int size_kerroin = int(float(size) / kerroin);

    float kerroin_minus_one = kerroin - 1.0;

    float ker_min1_pos = kerroin_minus_one < 0 ? 0 : kerroin_minus_one;

    int size_min_size_ker_pos = size - size_kerroin < 0 ? 0 : size - size_kerroin;

    if (index_write1 > size_min_size_ker_pos) 
    {
        int temp1 = int(float(index_write1 - size_min_size_ker_pos) * kerroin);
        output += buffer[temp1] * tanhWindower(temp1) * (ker_min1_pos + 0.2 + (windower(temp1) * 0.8 * (1.0 - ker_min1_pos)));
    }

    if (index_write2 > size_min_size_ker_pos)
    {
        int temp2 = int(float(index_write2 - size_min_size_ker_pos) * kerroin);
        output += buffer[temp2 + buf_size] * tanhWindower(temp2) * (ker_min1_pos + (windower(temp2) * (1.0 - ker_min1_pos))) * (ker_min1_pos * 0.75 + 0.25);
    }

    if (index_write3 > size_min_size_ker_pos)
    {
        int temp3 = int(float(index_write3 - size_min_size_ker_pos) * kerroin);
        output += buffer[temp3 + buf_size * 2] * tanhWindower(temp3) * (ker_min1_pos + 0.2 + (windower(temp3) * 0.8 * (1.0 - ker_min1_pos)));
    }

    if (index_write4 > size_min_size_ker_pos)
    {
        int temp4 = int(float(index_write4 - size_min_size_ker_pos) * kerroin);
        output += buffer[temp4 + buf_size * 3] * tanhWindower(temp4) * (ker_min1_pos + (windower(temp4) * (1.0 - ker_min1_pos))) * (ker_min1_pos * 0.75 + 0.25);
    }
  
   return output;
}
