#ifndef LFO_H_
#define LFO_H_

class LfoLP
{
    private:
        float speed;
        float depth;
        float index;
        float samplerate;

    public:
        LfoLP() {}
        ~LfoLP() {}

        void Init(float samplerate_, float speed_, float depth_);

        float getNextRaw();
        void setSpeed(float speed_);
        void setDepth(float depth_);
        float getTreatedNext();
        float getFromZero();
        float getDepth();
        float getSpeed();
};

#endif