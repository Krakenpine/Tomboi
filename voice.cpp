#include "voice.h"
#include "sample.h"
#include "adsrLP.h"
#include "lfoLP.h"
#include <string>

void Voice::Init(Sample sound1_, Sample sound2_,  float sound2_volume_, float sound2_detune_)
{ 
    sound1 = sound1_;
    sound2 = sound2_;

    sound2_volume = sound2_volume_;
    sound2_detune = sound2_detune_;

    age = 0;
    envelope1.Init(48000.0, 0.01, 0.11, 0.75, 0.55);
    envelope2.Init(48000.0, 0.42, 0.12, 0.75, 0.51);
    lfo1.Init(48000, 0.20, 0.0);
    lfo2.Init(48000, 0.333, 0.0);

    soundStruct sound1S;
    sound1S.sound = sound1_;
    sound1S.envelope = envelope1;
    sound1S.lfo = lfo1;
    sound1S.volume = 1.0;
    sound1S.detune = 0.0;

    soundStruct sound2S;
    sound2S.sound = sound2_;
    sound2S.envelope = envelope2;
    sound2S.lfo = lfo2;
    sound2S.volume = 1.0;
    sound2S.detune = 0.0;

    sounds.push_back(sound1S);
    sounds.push_back(sound2S);
}

void Voice::Init(Sample sound1_)
{ 
    age = 0;
    envelope1.Init(48000.0, 0.001, 0.001, 1.00, 0.005);
    lfo1.Init(48000, 0.20, 0.0);

    soundStruct sound1S;
    sound1S.sound = sound1_;
    sound1S.envelope = envelope1;
    sound1S.lfo = lfo1;
    sound1S.volume = 1.0;
    sound1S.detune = 0.0;

    sounds.push_back(sound1S);
}

void Voice::setAttack(float attack)
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.setAttack(attack);
    }
}

void Voice::setDecay(float decay)
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(0).envelope.setDecay(decay);
    }
}

void Voice::setSustain(float sustain)
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.setSustain(sustain);
    }
}

void Voice::setRelease(float release)
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.setRelease(release);
    }
}

void Voice::start()
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.start();
        sounds.at(i).sound.start();
    }
}

void Voice::startWithExistingSpeed()
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.start();
        sounds.at(i).sound.startWithExistingSpeed();
    }
}

float Voice::getNextSample()
{
    float output = 0;
    for (int i = 0; i < int(sounds.size()); i++)
    {
        if (sounds.at(i).envelope.isRunning() || sounds.at(i).sound.isPlaying())
        {
            float lfo_sample = sounds.at(i).lfo.getTreatedNext();
            float sound_sample = sounds.at(i).sound.getNextSample(lfo_sample + sounds.at(i).detune) * sounds.at(i).envelope.getNext();
            output += sound_sample;
        }
    }
    return output;
}

void Voice::setFrequency(float freq)
{
    frequency = freq;
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).sound.setFrequency(frequency);
    }
}

void Voice::setSpeed(float speed)
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).sound.setSpeed(speed);
    }
}

void Voice::setMidiNote(int midi)
{
    midi_note = midi;
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).sound.setMidiNote(midi_note);
    }
}

void Voice::midiNoteOff()
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.stop();
    }
}

void Voice::stop()
{
    for (int i = 0; i < int(sounds.size()); i++)
    {
        sounds.at(i).envelope.stop();
        sounds.at(i).sound.stop();
    }
}

bool Voice::isPlaying() {
    bool is_playing = false;
    for (int i = 0; i < int(sounds.size()); i++)
    {
        if (sounds.at(0).envelope.isRunning())
        {
            is_playing = true;
        }
    }
    return is_playing;
}

int Voice::getAge()
{
    return age;
}

void Voice::setAge(int new_age)
{
    age = new_age;
}

int Voice::getMidiNote()
{
    return midi_note;
}

bool Voice::envelopesInRelease()
{
    bool in_release = false;
    for (int i = 0; i < int(sounds.size()); i++)
    {
        if (sounds.at(0).envelope.isInRelease())
        {
            in_release = true;
        }
    }
    return in_release;
}

/*
class Voice {
    private:
        Sample sound1;
        Sample sound2;
        LfoLP lfo1;
        LfoLP lfo2;
        ADSR envelope1;
        ADSR envelope2;

        float sound2_volume;
        float sound2_detune;

        float frequency = 0;

        int midi_note = 0;

        int age;
    
    public:
        Voice(  Sample t1,
                Sample t2,
                float s2_vol,
                float s2_dt) :
                sound1(t1),
                sound2(t2),
                sound2_volume(s2_vol),
                sound2_detune(s2_dt) { 
            age = 0;
            envelope1.Init(48000.0, 0.01, 0.01, 0.75, 0.05);
            envelope2.Init(48000.0, 0.02, 0.02, 0.75, 0.01);
            lfo1.Init(48000, 0.20, 0.0);
            lfo2.Init(48000, 0.333, 0.0);
        }

        void setAttack(float attack)
        {
            envelope1.setAttack(attack);
            envelope2.setAttack(attack);
        }

        void setDecay(float decay)
        {
            envelope1.setDecay(decay);
            envelope2.setDecay(decay);
        }

        void setSustain(float sustain)
        {
            envelope1.setSustain(sustain);
            envelope2.setSustain(sustain);
        }

        void setRelease(float release)
        {
            envelope1.setRelease(release);
            envelope2.setRelease(release);
        }

        std::string start()
        {
            envelope1.start();
            std::string sound1_string = sound1.start();
            envelope2.start();
            sound2.start();

            return sound1_string;
        }

        float getNextSample()
        {
            float lfo_sample1 = lfo1.getTreatedNext();
            float lfo_sample2 = lfo2.getTreatedNext();

            float output = 0;

            if (envelope1.isRunning())
            {
                float sound_1_sample = sound1.getNextSample(lfo_sample1) * envelope1.getNext();
                output += sound_1_sample;
            }

            if (envelope2.isRunning())
            {
                float sound_2_sample = sound2.getNextSample(lfo_sample2 + sound2_detune) * envelope2.getNext() * sound2_volume;
                output += sound_2_sample;
            }

            return output;

        }

        void setFrequency(float freq)
        {
            frequency = freq;
            sound1.setFrequency(frequency);
            sound2.setFrequency(frequency * sound2_detune);
        }

        void setMidiNote(int midi)
        {
            midi_note = midi;
            sound1.setMidiNote(midi_note);
            sound2.setMidiNote(midi_note);
        }

        void setSpeed(float speed)
        {
            sound1.setSpeed(speed);
            sound2.setSpeed(speed * (1 + sound2_detune));
        }

        void addNoteToSpeed(float note)
        {
            sound1.addNoteToSpeed(note);
            sound2.addNoteToSpeed(note);
        }

        void midiNoteOff()
        {
            envelope1.stop();
            envelope2.stop();
        }

        void stop()
        {
            envelope1.stop();
            envelope2.stop();
            sound1.stop();
            sound2.stop();
        }

        bool isPlaying() {
            return sound1.isPlaying() || sound2.isPlaying() || envelope1.isRunning() || envelope2.isRunning();
        }
        
        int getAge()
        {
            return age;
        }

        void setAge(int new_age)
        {
            age = new_age;
        }

        int getMidiNote()
        {
            return midi_note;
        }

        bool envelopesInRelease()
        {
            return envelope1.isInRelease() || envelope2.isInRelease();
        }

};*/