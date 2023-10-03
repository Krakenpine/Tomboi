#ifndef SAMPLE_H_
#define SAMPLE_H_

#include <vector>
#include <string>

class Sample
{
    private:
        bool playing;
        float speed;
        float index;
        bool loopable;

        std::vector <const float *> samples;
        std::vector <int> array_sizes;
        std::vector <int> loop_points;
        std::vector <float> base_freqs;

        std::vector <const char*> names;

        int selected_sample;

        float frequency;

        int midi_note;

        std::string name_;

        bool fade;

    public:
        Sample() {}
        ~Sample() {}

        void Init(bool loopable_);
        void addSample(const float* arr, int size, int loop, float hz, const char* name);
        float getSample(float value);
        float frequencyToSpeed();
        float getNextSample(float speed_mod);
        float getHz();
        bool isPlaying();
        void setFrequency(float freq);
        void setMidiNote(int midi);
        void start();
        void stop();
        float getSpeed();
        void setSpeed();
        void setSpeed(float speed);
        void startWithExistingSpeed();
};

#endif