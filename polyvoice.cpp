#include <vector>
#include <climits>
#include "voice.h"
#include "polyvoice.h"

void PolyVoice::Init(Voice voice_, int polyphony)
{
    newest_age = 0;

    for (int i = 0; i < polyphony; i++)
    {
        voices.push_back(voice_);
        steal_counters.push_back(0);
        steal_freqs.push_back(0);
        steal_notes.push_back(0);
    }
}

int PolyVoice::getNumberOfVoicesPlaying()
{
    int temp = 0;
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        if (voices.at(i).isPlaying())
        {
            temp++;
        }
    }
    return temp;
}

int PolyVoice::getVoiceMaxCount()
{
    return int(voices.size());
}

int PolyVoice::getSoundAmount()
{
    return 0;
    //return voices.at(0).getSoundAmount();
}

void PolyVoice::stopAll()
{
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        voices.at(i).stop();
    }
}

void PolyVoice::setAttack(float attack)
{
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        voices.at(i).setAttack(attack);
    }
}

void PolyVoice::setDecay(float decay)
{
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        voices.at(i).setDecay(decay);
    }
}

void PolyVoice::setSustain(float sustain)
{
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        voices.at(i).setSustain(sustain);
    }
}

void PolyVoice::setRelease(float release)
{
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        voices.at(i).setRelease(release);
    }
}


float PolyVoice::getSamples()
{
    float temp = 0;
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        temp += voices.at(i).getNextSample();
        /*
        if (steal_counters[i] > 0)
        {
            temp += voices.at(i).getNextSample() * steal_counters.at(i);
            steal_counters.at(i) = steal_counters.at(i) - 0.01f;
            if (steal_counters.at(i) <= 0)
            {
                stealCounterHasStopped(i);
            }
        }
        else
        {
            temp += voices.at(i).getNextSample();
        }
        */
    }
    return temp;
}

int PolyVoice::getLowestNoteIndex()
{
    int lowest_note = 128;
    int lowest_note_index = 0;
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        if (voices.at(i).getMidiNote() < lowest_note && voices.at(i).isPlaying())
        {
            lowest_note = voices.at(i).getMidiNote();
            lowest_note_index = i;
        }
    }
    return lowest_note_index;
}

int PolyVoice::getHighestNoteIndex()
{
    int highest_note = 0;
    int highest_note_index = 0;
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        if (voices.at(i).getMidiNote() < highest_note && voices.at(i).isPlaying())
        {
            highest_note = voices.at(i).getMidiNote();
            highest_note_index = i;
        }
    }
    return highest_note_index;
}

void PolyVoice::playVoice(float freq, int midi_note)
{
    int free_voice = -1;
    int oldest_voice_age = INT_MAX;
    int oldest_voice_index = 0;
    int oldest_voice_age_in_release = INT_MAX;
    int oldest_voice_in_release_index = -1;
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        if (!voices.at(i).isPlaying())
        {
            free_voice = i;
        }
        if (voices.at(i).envelopesInRelease() && voices.at(i).getAge() < oldest_voice_age_in_release &&
            i != getLowestNoteIndex() && i != getHighestNoteIndex())
        {
            oldest_voice_in_release_index = i;
            oldest_voice_age_in_release = voices.at(i).getAge();
        }
        if (voices.at(i).getAge() < oldest_voice_age  && i != getLowestNoteIndex() && i != getHighestNoteIndex())
        {
            oldest_voice_index = i;
            oldest_voice_age = voices.at(i).getAge();
        }
    }
    newest_age++;
    if (free_voice != -1)
    {
        voices.at(free_voice).setAge(newest_age);
        voices.at(free_voice).setMidiNote(midi_note);
        voices.at(free_voice).setFrequency(freq);
        voices.at(free_voice).start();
    }
    else if (oldest_voice_in_release_index != -1)
    {
        voices.at(oldest_voice_in_release_index).setAge(newest_age);
        voices.at(oldest_voice_in_release_index).setMidiNote(midi_note);
        voices.at(oldest_voice_in_release_index).setFrequency(freq);
        voices.at(oldest_voice_in_release_index).start();
    }
    else
    {
        voices.at(oldest_voice_index).setAge(newest_age);
        voices.at(oldest_voice_index).setMidiNote(midi_note);
        voices.at(oldest_voice_index).setFrequency(freq);
        voices.at(oldest_voice_index).start();
    }
}

void PolyVoice::playVoiceWithOnlySpeed(float speed)
{
    int oldest_voice_age = INT_MAX;
    int oldest_voice_index = 0;
    /*
    for (int i = 0; i < static_cast<int>(voices.size()); i++)
    {
        if (voices.at(i).getAge() < oldest_voice_age)
        {
            oldest_voice_index = i;
            oldest_voice_age = voices.at(i).getAge();
        }
    }
    */
    newest_age++;
    voices.at(oldest_voice_index).setAge(newest_age);
    voices.at(oldest_voice_index).setSpeed(speed);
    voices.at(oldest_voice_index).startWithExistingSpeed();
}

void PolyVoice::stealCounterHasStopped(int steal_index)
{
    steal_counters.at(steal_index) = 0;
    voices.at(steal_index).setMidiNote(steal_notes.at(steal_index));
    voices.at(steal_index).setFrequency(steal_freqs.at(steal_index));
    voices.at(steal_index).start();
}

void PolyVoice::stopVoice(int midi_note)
{
    for (int i = 0; i < (int)voices.size(); i++)
    {
        if (voices.at(i).getMidiNote() == midi_note)
        {
            voices.at(i).stop();
        }
    }
}

/*
class PolyVoice {
    private:
        Voice voice1;
        Voice voice2;
        Voice voice3;
        Voice voice4;
        Voice voice5;
        Voice voice6;
        Voice voice7;
        Voice voice8;
        std::vector <Voice> voices;

        int newest_age;

        std::vector <int> steal_counters;
        std::vector <float> steal_freqs;
        std::vector <int> steal_notes;
        
    public:
        PolyVoice();

        PolyVoice(Voice v1, Voice v2, Voice v3, Voice v4, Voice v5, Voice v6, Voice v7, Voice v8) : 
        voice1(v1), voice2(v2), voice3(v3), voice4(v4), voice5(v5), voice6(v6), voice7(v7), voice8(v8) {
            voices.push_back(voice1);
            voices.push_back(voice2);
            voices.push_back(voice3);
            voices.push_back(voice4);
            voices.push_back(voice5);
            voices.push_back(voice6);
            voices.push_back(voice7);
            voices.push_back(voice8);

            newest_age = 0;

            for (int i = 0; i < 8; i++)
            {
                steal_counters.push_back(-1);
                steal_freqs.push_back(0);
                steal_notes.push_back(0);
            }
        }
        
        int getVoiceMaxCount()
        {
            return voices.size();
        }

        void setAttack(float attack)
        {
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                voices.at(i).setAttack(attack);
            }
        }

        void setDecay(float decay)
        {
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                voices.at(i).setDecay(decay);
            }
        }

        void setSustain(float sustain)
        {
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                voices.at(i).setSustain(sustain);
            }
        }

        void setRelease(float release)
        {
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                voices.at(i).setRelease(release);
            }
        }


        float getSamples()
        {
            float temp = 0;
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                if (steal_counters[i] >= 0)
                {
                    temp += voices.at(i).getNextSample() * (static_cast<float>(steal_counters.at(i)) / 1000.0);

                    steal_counters.at(i) = steal_counters.at(i) - 1;
                    if (steal_counters.at(i) < 0)
                    {
                        stealCounterHasStopped(i);
                    }
                }
                else
                {
                    temp += voices.at(i).getNextSample();
                }
            }

            return temp;
        }

        int getLowestNoteIndex()
        {
            int lowest_note = 128;
            int lowest_note_index = 0;
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                if (voices.at(i).getMidiNote() < lowest_note && voices.at(i).isPlaying())
                {
                    lowest_note = voices.at(i).getMidiNote();
                    lowest_note_index = i;
                }
            }
            return lowest_note_index;
        }

        int getHighestNoteIndex()
        {
            int highest_note = 0;
            int highest_note_index = 0;
            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                if (voices.at(i).getMidiNote() < highest_note && voices.at(i).isPlaying())
                {
                    highest_note = voices.at(i).getMidiNote();
                    highest_note_index = i;
                }
            }
            return highest_note_index;
        }

        void playVoice(float freq, int midi_note)
        {
            int free_voice = -1;
            int oldest_voice_age = INT_MAX;
            int oldest_voice_index = 0;
            int oldest_voice_age_in_release = INT_MAX;
            int oldest_voice_in_release_index = -1;

            for (int i = 0; i < static_cast<int>(voices.size()); i++)
            {
                if (!voices.at(i).isPlaying())
                {
                    free_voice = i;
                }
                
                if (voices.at(i).envelopesInRelease() && voices.at(i).getAge() < oldest_voice_age_in_release &&
                    i != getLowestNoteIndex() && i != getHighestNoteIndex())
                {
                    oldest_voice_in_release_index = i;
                    oldest_voice_age_in_release = voices.at(i).getAge();
                }
                
                if (voices.at(i).getAge() < oldest_voice_age  && i != getLowestNoteIndex() && i != getHighestNoteIndex())
                {
                    oldest_voice_index = i;
                    oldest_voice_age = voices.at(i).getAge();
                }
                
            }

            newest_age++;

            if (free_voice != -1)
            {
                voices.at(free_voice).setAge(newest_age);
                voices.at(free_voice).setMidiNote(midi_note);
                voices.at(free_voice).setFrequency(freq);
                voices.at(free_voice).start();
            }
            else if (oldest_voice_in_release_index != -1)
            {
                voices.at(oldest_voice_in_release_index).setAge(newest_age);
                steal_counters[oldest_voice_in_release_index] = 1000;
                steal_notes[oldest_voice_in_release_index] = midi_note;
                steal_freqs[oldest_voice_in_release_index] = freq;
            }
            else
            {
                voices.at(oldest_voice_index).setAge(newest_age);
                steal_counters.at(oldest_voice_index) = 1000;
                steal_notes.at(oldest_voice_index) = midi_note;
                steal_freqs.at(oldest_voice_index) = freq;
            }
        }

        void stealCounterHasStopped(int steal_index)
        {
            voices.at(steal_index).setMidiNote(steal_notes.at(steal_index));
            voices.at(steal_index).setFrequency(steal_freqs.at(steal_index));
            voices.at(steal_index).start();
        }

        void stopVoice(int midi_note)
        {
            for (int i = 0; i < (int)voices.size(); i++)
            {
                if (voices.at(i).getMidiNote() == midi_note)
                {
                    voices.at(i).stop();
                }
            }
        }
};
*/