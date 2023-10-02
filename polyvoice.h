#ifndef POLYVOICE_H_
#define POLYVOICE_H_

#include <vector>
#include <climits>
#include "voice.h"

class PolyVoice
{
    private:
        int newest_age;

        std::vector <Voice> voices;
        std::vector <float> steal_counters;
        std::vector <float> steal_freqs;
        std::vector <int> steal_notes;

    public:
        PolyVoice() {}
        ~PolyVoice() {}

        void Init(Voice voice_, int polyphony);
        void stopAll();
        void setAttack(float attack);
        void setDecay(float decay);
        void setSustain(float sustain);
        void setRelease(float release);
        float getSamples();
        int getLowestNoteIndex();
        int getHighestNoteIndex();
        void playVoice(float freq, int midi_note);
        void stealCounterHasStopped(int steal_index);
        void stopVoice(int midi_note);
        int getVoiceMaxCount();
        int getNumberOfVoicesPlaying();
        int getSoundAmount();
        void playVoiceWithOnlySpeed(float speed);
};

#endif