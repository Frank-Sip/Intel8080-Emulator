#ifndef SOUNDSDL_H
#define SOUNDSDL_H

#include <SDL3/SDL.h>

#include "Logger.h"
#include "Sound.h"

class SoundSDL : public Sound
{
    SDL_AudioSpec m_audioSpec;
    SDL_AudioStream* m_audioStream;
    std::shared_ptr<Logger> m_logger;

    public:
        SoundSDL();
        ~SoundSDL();

        virtual void Init() override;
        virtual void PlayTune() override;
        virtual void PauseTune() override;
        virtual void Deinit() override;
};

#endif
