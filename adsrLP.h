#ifndef ADSR_H_
#define ADSR_H_

class ADSR
{
    private:
        float attack;
        float decay;
        float sustain;
        float release;
        float samplerate;

        float counter;

        int stage;

    public:
        ADSR() { }
        ~ADSR() { }

        void Init(float samplerate_, float attack_, float decay_, float sustain_, float release_);
        void setAttack(float attack_);
        void setDecay(float decay_);
        void setSustain(float sustain_);
        void setRelease(float release_);
        float getNext();

        void start();
        void stop();
        bool isRunning();
        bool isInRelease();

};

#endif