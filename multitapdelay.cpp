#pragma once

#include <vector>

class MultiTapDelay {
    private:
        int index;
        int size;
        float length;
        int length_samples;
        float feedback;
        float mix;
        int samplerate;        

        float* buffer;

    public:
    /*
        size:           length of delay array in samples
        len:            length of delay from size, 0.0 - 1.0
        length_samples: length of delay in samples
        feedback:       how much if delayed signal is written back, 0.0 - 1.0
        mix:            amplitude of returned signal, 0.0 - 1.0
        index:          index of sample where the "write head" is
        samplerate:     samplerate of audio system

    */
        MultiTapDelay(int size_, float length_, float fb, float mix_, float* buff) : size(size_), length(length_), feedback(fb), mix(mix_), buffer(buff)
        {
            index = 0;
            length_samples = static_cast<int>(static_cast<float>(size) * length);
            samplerate = 48000;
            for (int i = 0; i < size; i++)
            {
                buffer[i] = 0;
            }
        }

        void setSamplerate(int samplerate_)
        {
            samplerate = samplerate;
        }

        int getSamplerate()
        {
            return samplerate;
        }

        // get maximum length of delay
        int getSize()
        {
            return size;
        }

        // set length as 0.0 - 1.0 from size, set length_samples too
        void setLength(float new_length)
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
        float getLength()
        {
            return length;
        }

        // return delay length as samples
        int getLengthSamples()
        {
            return length_samples;
        }

        // return delay length
        int getLengthMS()
        {
            return static_cast<int>((static_cast<float>(length_samples) / static_cast<float>(size)) * 1000.0);
        }

        // set feedback as fraction 0.0 - 1.0
        void setFeedback(float fb_new)
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
        float getFeedback()
        {
            return feedback;
        }

        // set mix as faction 0.0 - 1.0
        void setMix(float new_mix)
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

        float getMix()
        {
            return mix;
        }

        int getIndex()
        {
            return index;
        }

        float process(float sample)
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

            buffer[index] = sample + delayed_signal * feedback;

            return buffer[temp] * mix;
        }




};