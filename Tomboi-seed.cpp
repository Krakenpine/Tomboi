#include "daisy_pod.h"
#include "daisysp.h"
#include <math.h>
#include <vector>
#include <string>
#include "samples/tom1.cpp"
#include "samples/tom2.cpp"
#include "samples/tom3.cpp"
#include "samples/tom4.cpp"
#include "samples/tom5.cpp"
#include "samples/tom6.cpp"
#include "samples/tom7.cpp"

#include "midi_notes.cpp"
#include "sample.h"
#include "voice.h"
#include "polyvoice.h"
#include "delayLP.h"
#include "chorusLP.h"
#include "tomvoice.h"

float DSY_SDRAM_BSS tom1_ram[tom1_size];
float DSY_SDRAM_BSS tom2_ram[tom2_size];
float DSY_SDRAM_BSS tom3_ram[tom3_size];
float DSY_SDRAM_BSS tom4_ram[tom4_size];
float DSY_SDRAM_BSS tom5_ram[tom5_size];
float DSY_SDRAM_BSS tom6_ram[tom6_size];
float DSY_SDRAM_BSS tom7_ram[tom7_size];

using namespace daisy;
using namespace daisysp;

DaisySeed  hardware;
ReverbSc   verb;

float DSY_SDRAM_BSS buf_pitch[48000 * 4] = { 0 };

float DSY_SDRAM_BSS buf_del[48000] = { 0 };

DelayLP delay1;

float DSY_SDRAM_BSS buf_cho[4800] = { 0 };
float DSY_SDRAM_BSS buf_cho_del_pitch[48000 * 4] = { 0 };

DelayLP delay_cho;

ChorusLP chorus1;

std::vector <TomVoice> tom_voices1;
std::vector <TomVoice> tom_voices2;
std::vector <TomVoice> tom_voices3;

float base_note = 16.35;

float drywet = 0.50;

float verb_feedback = 0.90;

float verb_lowpass = 18000.0;

int dry_feedback_lowpass = 0;

int selected_sample = 0;

bool pitch_type = false;

int selected_tom1 = 0;
int selected_tom2 = 1;
int selected_tom3 = 2;

float tom_speed1 = 1.0f;
float tom_speed2 = 1.0f;
float tom_speed3 = 1.0f;

Switch trigger1;
Switch trigger2;
Switch trigger3;
Switch cv_type;

void GetReverbSample(float &outl, float &outr, float inl, float inr);

float fastTanh(float in)
{
    if (in < -3.f)
    {
        return -1.f;
    }
    else if (in > 3.f)
    {
        return 1.f;
    }
    else
    {
        float in2 = in*in;
        return in * (27.f + in2) / (27.f + in2 * 9.f);
    }
}

int inputPitchToMidiNote(float pitch)
{
    // Voltage from 0 to 10 volts => 10 octaves + 1 semitone C-1 -> C9, 121 midi notes
    float lowest_midi_note = 0.0f; // C-1, midi note 0, 8.18 Hz
    float midi_note_range = 121.0f;
    return static_cast<int>(lowest_midi_note + pitch * midi_note_range);
}

static void  audio(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    trigger1.Debounce();
    bool trigger1_pushed = trigger1.FallingEdge();

    trigger2.Debounce();
    bool trigger2_pushed = trigger2.FallingEdge();

    trigger3.Debounce();
    bool trigger3_pushed = trigger3.FallingEdge();

    float speed1 = 1.0f - hardware.adc.GetFloat(0);
    float speed2 = 1.0f - hardware.adc.GetFloat(2);
    float speed3 = 1.0f - hardware.adc.GetFloat(4);

    if (trigger1_pushed)
    {
        selected_tom1 = static_cast<int>((hardware.adc.GetFloat(1)) * 7.0f);
        if (speed1 >= 0.5)
        {
            tom_speed1 = 1.0f + (speed1 - 0.5f) * 6.0f;
        }
        else
        {
            tom_speed1 = 1.0f / ((1.0f - speed1 * 2.0f) * 3.0f + 1.0f);
        }
        tom_voices1.at(selected_tom1).setSpeed(tom_speed1);
        tom_voices1.at(selected_tom1).start();
    }

    if (trigger2_pushed)
    {
        selected_tom2 = static_cast<int>((hardware.adc.GetFloat(3)) * 7.0f);
        if (speed2 >= 0.5)
        {
            tom_speed2 = 1.0f + (speed2 - 0.5f) * 6.0f;
        }
        else
        {
            tom_speed2 = 1.0f / ((1.0f - speed2 * 2.0f) * 3.0f + 1.0f);
        }
        tom_voices2.at(selected_tom2).setSpeed(tom_speed2);
        tom_voices2.at(selected_tom2).start();
    }

    if (trigger3_pushed)
    {
        selected_tom3 = int((hardware.adc.GetFloat(5)) * 7.0f);
        if (speed3 >= 0.5)
        {
            tom_speed3 = 1.0f + (speed3 - 0.5f) * 6.0f;
        }
        else
        {
            tom_speed3 = 1.0f / ((1.0f - speed3 * 2.0f) * 3.0f + 1.0f);
        }
        tom_voices3.at(selected_tom3).setSpeed(tom_speed3);
        tom_voices3.at(selected_tom3).start();
    }

    cv_type.Debounce();
    if (cv_type.Pressed())
    {
        tom_voices1.at(selected_tom1).setSpeed(tom_speed1);
        tom_voices2.at(selected_tom2).setSpeed(tom_speed2);
        tom_voices3.at(selected_tom3).setSpeed(tom_speed3);
    }

    drywet = hardware.adc.GetFloat(6) * 0.5f;
    verb_feedback = 0.5f + hardware.adc.GetFloat(7) * 0.5f;
    verb_lowpass = 18000.0 * hardware.adc.GetFloat(8);

    verb.SetFeedback(verb_feedback);
    verb.SetLpFreq(verb_lowpass);

    // Audio Loop

    float outl, outr;

    for(size_t i = 0; i < size; i += 2)
    {
        // Process

        float temp = 0;

        temp += tom_voices1.at(selected_tom1).getNextSample();
        temp += tom_voices2.at(selected_tom2).getNextSample();
        temp += tom_voices3.at(selected_tom3).getNextSample();

        temp *= 0.5;

        // Chorus and delay are not used in current sounds
        // There could be separate chorus and delay for different voices
        //temp = chorus1.process(temp);

        //temp += delay1.process(temp);

        GetReverbSample(outl, outr, temp, temp);

        outl = fastTanh(outl);
        outr = fastTanh(outr);
        out[i]     = outl;
        out[i + 1] = outr;
    }
}

void GetReverbSample(float &outl, float &outr, float inl, float inr)
{
    verb.Process(inl, inr, &outl, &outr);
    outl = drywet * outl + (1 - drywet) * inl;
    outr = drywet * outr + (1 - drywet) * inr;
}

void InitSynth(float samplerate)
{
    verb.Init(samplerate);

    verb.SetLpFreq(18000.0f);
    verb.SetFeedback(0.80f);

    delay1.Init(48000, 0.5, 0.5, 0.0, false, buf_del, buf_pitch);

    chorus1.Init(0.0, 0.5, buf_cho, buf_cho_del_pitch);

    for (int i = 0; i < tom1_size; i++) { tom1_ram[i] = static_cast<float>(tom1[i]) / 32767.0; }
    for (int i = 0; i < tom2_size; i++) { tom2_ram[i] = static_cast<float>(tom2[i]) / 32767.0; }
    for (int i = 0; i < tom3_size; i++) { tom3_ram[i] = static_cast<float>(tom3[i]) / 32767.0; }
    for (int i = 0; i < tom4_size; i++) { tom4_ram[i] = static_cast<float>(tom4[i]) / 32767.0; }
    for (int i = 0; i < tom5_size; i++) { tom5_ram[i] = static_cast<float>(tom5[i]) / 32767.0; }
    for (int i = 0; i < tom6_size; i++) { tom6_ram[i] = static_cast<float>(tom6[i]) / 32767.0; }
    for (int i = 0; i < tom7_size; i++) { tom7_ram[i] = static_cast<float>(tom7[i]) / 32767.0; }

    System::Delay(500);

    Sample tom1_sample;
    tom1_sample.Init(false);
    tom1_sample.addSample(tom1_ram, tom1_size, tom1_loop, tom1_tune, "tom1");
    TomVoice tom1_voice;
    tom1_voice.Init(tom1_sample);
    tom_voices1.push_back(tom1_voice);
    tom_voices2.push_back(tom1_voice);
    tom_voices3.push_back(tom1_voice);

    Sample tom2_sample;
    tom2_sample.Init(false);
    tom2_sample.addSample(tom2_ram, tom2_size, tom2_loop, tom2_tune, "tom2");
    TomVoice tom2_voice;
    tom2_voice.Init(tom2_sample);
    tom_voices1.push_back(tom2_voice);
    tom_voices2.push_back(tom2_voice);
    tom_voices3.push_back(tom2_voice);

    Sample tom3_sample;
    tom3_sample.Init(false);
    tom3_sample.addSample(tom3_ram, tom3_size, tom3_loop, tom3_tune, "tom3");
    TomVoice tom3_voice;
    tom3_voice.Init(tom3_sample);
    tom_voices1.push_back(tom3_voice);
    tom_voices2.push_back(tom3_voice);
    tom_voices3.push_back(tom3_voice);

    Sample tom4_sample;
    tom4_sample.Init(false);
    tom4_sample.addSample(tom4_ram, tom4_size, tom4_loop, tom4_tune, "tom4");
    TomVoice tom4_voice;
    tom4_voice.Init(tom4_sample);
    tom_voices1.push_back(tom4_voice);
    tom_voices2.push_back(tom4_voice);
    tom_voices3.push_back(tom4_voice);

    Sample tom5_sample;
    tom5_sample.Init(false);
    tom5_sample.addSample(tom5_ram, tom5_size, tom5_loop, tom5_tune, "tom5");
    TomVoice tom5_voice;
    tom5_voice.Init(tom5_sample);
    tom_voices1.push_back(tom5_voice);
    tom_voices2.push_back(tom5_voice);
    tom_voices3.push_back(tom5_voice);

    Sample tom6_sample;
    tom6_sample.Init(false);
    tom6_sample.addSample(tom6_ram, tom6_size, tom6_loop, tom6_tune, "tom6");
    TomVoice tom6_voice;
    tom6_voice.Init(tom6_sample);
    tom_voices1.push_back(tom6_voice);
    tom_voices2.push_back(tom6_voice);
    tom_voices3.push_back(tom6_voice);

    Sample tom7_sample;
    tom7_sample.Init(false);
    tom7_sample.addSample(tom7_ram, tom7_size, tom7_loop, tom7_tune, "tom7");
    TomVoice tom7_voice;
    tom7_voice.Init(tom7_sample);
    tom_voices1.push_back(tom7_voice);
    tom_voices2.push_back(tom7_voice);
    tom_voices3.push_back(tom7_voice);

}

void initKnobs()
{

    AdcChannelConfig adc[9];

    adc[0].InitSingle(hardware.GetPin(15));     // ADC0 pitch 1
    adc[1].InitSingle(hardware.GetPin(16));     // ADC1 select 1
    adc[2].InitSingle(hardware.GetPin(17));     // ADC2 pitch 2
    adc[3].InitSingle(hardware.GetPin(18));     // ADC3 select 2
    adc[4].InitSingle(hardware.GetPin(24));     // ADC9 pitch 3
    adc[5].InitSingle(hardware.GetPin(20));     // ADC5 select 3
    adc[6].InitSingle(hardware.GetPin(21));     // ADC6 reverb mix
    adc[7].InitSingle(hardware.GetPin(22));     // ADC7 reverb length
    adc[8].InitSingle(hardware.GetPin(23));     // ADC8 reverb lowpass

    hardware.adc.Init(adc, 9);
    hardware.adc.Start();

    trigger1.Init(hardware.GetPin(11), 1000);
    trigger2.Init(hardware.GetPin(12), 1000);
    trigger3.Init(hardware.GetPin(13), 1000);
    cv_type.Init(hardware.GetPin(14), 1000);
}

int main(void)
{
    float samplerate;
    // Init
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioBlockSize(32);
    samplerate = hardware.AudioSampleRate();

    System::Delay(500);

    hardware.StartLog();
    InitSynth(samplerate);

    // Start Callbacks
    initKnobs();
    hardware.StartAudio(audio);

    while(1)
    {

    }
}
