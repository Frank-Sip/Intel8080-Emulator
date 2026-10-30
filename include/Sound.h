#ifndef SOUND_H
#define SOUND_H

class Sound
{
public:
    virtual void Init() = 0;
    virtual void PlayTune() = 0;
    virtual void PauseTune() = 0;
    virtual void Deinit() = 0;
};

#endif