#include "SoundSDL.h"

#include <cstdlib>
#include <string>

SoundSDL::SoundSDL() : m_audioDevice(0), m_audioStreams{}, m_audioBuffers{}, m_audioLengths{}, m_ufoActive(false), m_logger(Logger::GetInstance()) {}

SoundSDL::~SoundSDL() = default;

void SoundSDL::Init()
{
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        std::string errorSDL(SDL_GetError());
        m_logger->Log("Failed to initialize SDL audio: " + errorSDL, ELogLevel::ERROR);
        std::exit(1);
    }

    m_audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

    if (m_audioDevice == 0)
    {
        m_logger->Log("Failed to open audio device: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    LoadSound(0, "Sounds/ufo_highpitch.wav");
    LoadSound(1, "Sounds/shoot.wav");
    LoadSound(2, "Sounds/explosion.wav");
    LoadSound(3, "Sounds/invaderkilled.wav");
    LoadSound(4, "Sounds/fastinvader1.wav");
    LoadSound(5, "Sounds/fastinvader2.wav");
    LoadSound(6, "Sounds/fastinvader3.wav");
    LoadSound(7, "Sounds/fastinvader4.wav");

    if (!SDL_ResumeAudioDevice(m_audioDevice))
    {
        m_logger->Log("Failed to resume audio device: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }
}

void SoundSDL::PlaySound(uint8_t soundId)
{
    if (soundId >= SOUND_COUNT) return;

    if (soundId == UFO_SOUND_ID) m_ufoActive = true;

    if (!SDL_ClearAudioStream(m_audioStreams[soundId]))
    {
        m_logger->Log("Failed to clear audio stream: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        return;
    }

    QueueSound(soundId);

    if (soundId == UFO_SOUND_ID) QueueSound(soundId);
}

void SoundSDL::StopSound(uint8_t soundId)
{
    if (soundId >= SOUND_COUNT) return;

    if (soundId == UFO_SOUND_ID) m_ufoActive = false;

    if (!SDL_ClearAudioStream(m_audioStreams[soundId]))
    {
        m_logger->Log("Failed to stop audio stream: " + std::string(SDL_GetError()), ELogLevel::ERROR);
    }
}

void SoundSDL::Update()
{
    if (!m_ufoActive) return;

    const int queuedBytes = SDL_GetAudioStreamQueued(m_audioStreams[UFO_SOUND_ID]);

    if (queuedBytes < 0)
    {
        m_logger->Log("Failed to query UFO audio stream: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        return;
    }

    if (queuedBytes < static_cast<int>(m_audioLengths[UFO_SOUND_ID] * 2))
    {
        QueueSound(UFO_SOUND_ID);
    }
}

void SoundSDL::Deinit()
{
    m_ufoActive = false;

    for (std::size_t soundId = 0; soundId < SOUND_COUNT; soundId++)
    {
        if (m_audioStreams[soundId])
        {
            SDL_DestroyAudioStream(m_audioStreams[soundId]);
            m_audioStreams[soundId] = nullptr;
        }

        if (m_audioBuffers[soundId])
        {
            SDL_free(m_audioBuffers[soundId]);
            m_audioBuffers[soundId] = nullptr;
        }

        m_audioLengths[soundId] = 0;
    }

    if (m_audioDevice != 0)
    {
        SDL_CloseAudioDevice(m_audioDevice);
        m_audioDevice = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SoundSDL::LoadSound(uint8_t soundId, const char* filePath)
{
    SDL_AudioSpec audioSpec{};

    if (!SDL_LoadWAV(filePath, &audioSpec, &m_audioBuffers[soundId], &m_audioLengths[soundId]))
    {
        m_logger->Log("Failed to load sound " + std::string(filePath) + ": " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    m_audioStreams[soundId] = SDL_CreateAudioStream(&audioSpec, nullptr);

    if (!m_audioStreams[soundId])
    {
        m_logger->Log("Failed to create audio stream for " + std::string(filePath) + ": " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    if (!SDL_BindAudioStream(m_audioDevice, m_audioStreams[soundId]))
    {
        m_logger->Log("Failed to bind audio stream for " + std::string(filePath) + ": " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }
}

void SoundSDL::QueueSound(uint8_t soundId)
{
    if (!SDL_PutAudioStreamData(m_audioStreams[soundId], m_audioBuffers[soundId], static_cast<int>(m_audioLengths[soundId])))
    {
        m_logger->Log("Failed to queue sound: " + std::string(SDL_GetError()), ELogLevel::ERROR);
    }
}
