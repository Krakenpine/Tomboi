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
