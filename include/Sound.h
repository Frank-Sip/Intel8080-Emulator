#ifndef SOUND_H
#define SOUND_H

#include <cstdint>

class Sound
{
public:
    virtual ~Sound() = default;

    virtual void Init() = 0;
    virtual void PlaySound(uint8_t soundId) = 0;
    virtual void StopSound(uint8_t soundId) = 0;
    virtual void Update() = 0;
    virtual void Deinit() = 0;
};

#endif
