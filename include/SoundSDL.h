#ifndef SOUNDSDL_H
#define SOUNDSDL_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <SDL3/SDL.h>

#include "Logger.h"
#include "Sound.h"

class SoundSDL : public Sound
{
    static constexpr std::size_t SOUND_COUNT = 8;
    static constexpr uint8_t UFO_SOUND_ID = 0;

    SDL_AudioDeviceID m_audioDevice;
    SDL_AudioStream* m_audioStreams[SOUND_COUNT];
    Uint8* m_audioBuffers[SOUND_COUNT];
    Uint32 m_audioLengths[SOUND_COUNT];
    bool m_ufoActive;

    std::shared_ptr<Logger> m_logger;

    void LoadSound(uint8_t soundId, const char* filePath);
    void QueueSound(uint8_t soundId);

public:
    SoundSDL();
    ~SoundSDL() override;

    void Init() override;
    void PlaySound(uint8_t soundId) override;
    void StopSound(uint8_t soundId) override;
    void Update() override;
    void Deinit() override;
};

#endif
