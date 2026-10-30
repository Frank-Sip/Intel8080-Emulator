#include <SDL3/SDL.h>

#include <cmath>
#include <cstdlib>
#include <numbers>
#include <string>

#include "SoundSDL.h"

namespace
{
constexpr int SAMPLE_RATE = 44100;
constexpr int AMPLITUDE = 28000;
constexpr double FREQUENCY = 440.0;
constexpr int CALLBACK_BUFFER_SAMPLES = 256;
}

void SDLCALL AudioCallback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount)
{
    (void)userdata;
    (void)totalAmount;

    static double phase = 0.0;
    constexpr double twoPi = 2.0 * std::numbers::pi_v<double>;
    constexpr double increment = (twoPi * FREQUENCY) / SAMPLE_RATE;

    int remainingSamples = additionalAmount / static_cast<int>(sizeof(Sint16));

    while (remainingSamples > 0)
    {
        Sint16 samples[CALLBACK_BUFFER_SAMPLES];
        const int sampleCount = remainingSamples < CALLBACK_BUFFER_SAMPLES ? remainingSamples : CALLBACK_BUFFER_SAMPLES;

        for (int i = 0; i < sampleCount; ++i)
        {
            samples[i] = static_cast<Sint16>(AMPLITUDE * std::sin(phase));
            phase += increment;

            if (phase >= twoPi)
            {
                phase -= twoPi;
            }
        }

        SDL_PutAudioStreamData(stream, samples, sampleCount * static_cast<int>(sizeof(Sint16)));

        remainingSamples -= sampleCount;
    }
}

SoundSDL::SoundSDL() : m_audioSpec{}, m_audioStream(nullptr), m_logger(Logger::GetInstance()) {}

SoundSDL::~SoundSDL() = default;

void SoundSDL::Init()
{
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        std::string errorSDL(SDL_GetError());
        m_logger->Log("Failed to initialize SDL audio: " + errorSDL, ELogLevel::ERROR);
        std::exit(1);
    }

    m_audioSpec.freq = SAMPLE_RATE;
    m_audioSpec.format = SDL_AUDIO_S16;
    m_audioSpec.channels = 1;

    m_audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec, AudioCallback, nullptr);

    if (!m_audioStream)
    {
        std::string errorSDL(SDL_GetError());
        m_logger->Log("Failed to open audio: " + errorSDL, ELogLevel::ERROR);
        std::exit(1);
    }
}

void SoundSDL::PlayTune()
{
    if (m_audioStream && !SDL_ResumeAudioStreamDevice(m_audioStream))
    {
        m_logger->Log("Failed to resume audio: " + std::string(SDL_GetError()), ELogLevel::ERROR);
    }
}

void SoundSDL::PauseTune()
{
    if (m_audioStream && !SDL_PauseAudioStreamDevice(m_audioStream))
    {
        m_logger->Log("Failed to pause audio: " + std::string(SDL_GetError()), ELogLevel::ERROR);
    }
}

void SoundSDL::Deinit()
{
    if (m_audioStream)
    {
        SDL_DestroyAudioStream(m_audioStream);
        m_audioStream = nullptr;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}