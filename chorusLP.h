#ifndef CHORUS_LP_H_
#define CHORUS_LP_H_

#include "delayLP.h"
#include "lfoLP.h"

class ChorusLP
{
    private:
        DelayLP delay;
        LfoLP lfo;

        float depth_min;
        float mix;

    public:
        ChorusLP() {}
        ~ChorusLP() {}

        void Init(float depth_min_, float mix_, float* buffer_delay, float* buffer_delay_pitch);
        void setDepth(float depth);
        void setSpeed(float speed);
        void setMix(float new_mix);
        void setMinDepth(float new_min);
        float getDepth();
        float getSpeed();
        float getMix();
        float getDepthMin();
        void setFeedback(float fb_new);
        float getFeedback();
        float process(float sample);
};

#endif