#ifndef PITCH_LP_H_
#define PITCH_LP_

#include "daisysp.h"
using namespace daisysp;

class PitchLP
{
    private:
        float* buffer;

        int size;

        int index_write;

        float amount;

        bool buffer_select;

        float sample_raw_prev;

        int buf_size;

        Svf filt1;

    public:
        PitchLP() {}
        ~PitchLP() {}

        void Init(float* buffer_);
        void setAmount(float amount_);
        float tanhWindower(int index);
        float windower(int index);
        float process(float sample_raw);
};


#endif