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
