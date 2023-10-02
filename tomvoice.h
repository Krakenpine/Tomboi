#ifndef TOMVOICE_H_
#define TOMVOICE_H_

#include "sample.h"

class TomVoice
{
    private:
        Sample sound;

        float speed;

    public:
        TomVoice() {}
        ~TomVoice() {}

        void Init(Sample sound_);
        void setSpeed(float speed_);
        void start();
        float getNextSample();

};

#endif