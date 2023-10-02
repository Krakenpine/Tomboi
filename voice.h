#ifndef VOICE_H_
#define VOICE_H_

#include "sample.h"
#include "adsrLP.h"
#include "lfoLP.h"
#include <string>
#include <vector>

struct soundStruct {
    Sample sound;
    LfoLP lfo;
    ADSR envelope;
    float volume;
    float detune;
};

class Voice
{
    private:
        Sample sound1;
        Sample sound2;
        LfoLP lfo1;
        LfoLP lfo2;
        ADSR envelope1;
        ADSR envelope2;

        float sound2_volume;
        float sound2_detune;

        float frequency;

        int midi_note;

        int age;

        std::vector <soundStruct> sounds;

    public:
        Voice() {}
        ~Voice() {}

        void Init();
        void Init(Sample sound1_);
        void Init(Sample sound1_, Sample sound2_);
        void Init(Sample sound1_, Sample sound2_, Sample sound3_);
        void Init(Sample sound1_, Sample sound2_, Sample sound3_, Sample sound4_);
        void Init(Sample sound1_, Sample sound2_, float sound2_volume_, float sound2_detune_);
        void setAttack(float attack);
        void setAttack(float attack, int sound);
        void setDecay(float decay);
        void setDecay(float decay, int sound);
        void setSustain(float sustain);
        void setSustain(float sustain, int sound);
        void setRelease(float release);
        void setRelease(float release, int sound);
        void setEnvelope(float attack, float decay, float sustain, float release);
        void setEnvelope(float attack, float decay, float sustain, float release, int sound);
        void start();
        float getNextSample();
        void setFrequency(float freq);
        void setMidiNote(int midi);
        void midiNoteOff();
        void stop();
        bool isPlaying();
        int getAge();
        void setAge(int new_age);
        int getMidiNote();
        bool envelopesInRelease();
        //int getSoundAmount();
        void startWithExistingSpeed();
        void setSpeed(float speed);
};

#endif