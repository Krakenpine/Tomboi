#ifndef DELAY_H_
#define DELAY_H_

#include <vector>
#include "pitchLP.h"

class DelayLP
{
    private:
        int index;
        int size;
        float length;
        int length_samples;
        float feedback;
        float mix;
        int samplerate;        

        float* buffer; 

        PitchLP pitch_shifter;

        bool pitch_enabled;

    public:
        DelayLP() {}
        ~DelayLP() {}

        /*
            size:           length of delay array in samples
            len:            length of delay from size, 0.0 - 1.0
            length_samples: length of delay in samples
            feedback:       how much if delayed signal is written back, 0.0 - 1.0
            mix:            amplitude of returned signal, 0.0 - 1.0
            index:          index of sample where the "write head" is
            samplerate:     samplerate of audio system

        */
        void Init(int size_, float length_, float fb, float mix_, bool pitch_enabled_, float* buffer_, float* buffer_pitch);

        void setSamplerate(int samplerate_);
        int getSamplerate();
        void setLength(float new_length);
        float getLength();
        int getLengthSamples();
        int getLengthMS();
        void setFeedback(float fb_new);
        float getFeedback();
        void setMix(float new_mix);
        int getSize();
        float getMix();
        int getIndex();
        void setDelayPitch(float pitch);
        float process(float sample);
};

#endif