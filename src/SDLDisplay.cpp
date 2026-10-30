#include "SDLDisplay.h"

#include <cstdlib>
#include <string>

DisplaySDL::DisplaySDL() : m_window(nullptr), m_surface(nullptr), m_logger(Logger::GetInstance()) {}

DisplaySDL::~DisplaySDL() = default;

void DisplaySDL::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        m_logger->Log("SDL initialization failed: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    m_window = SDL_CreateWindow("Space Invaders Emulator", SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, 0);

    if (!m_window)
    {
        m_logger->Log("Window creation failed: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    m_surface = SDL_GetWindowSurface(m_window);

    if (!m_surface)
    {
        m_logger->Log("Failed to get window surface: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }
}

void DisplaySDL::Draw(std::span<const uint8_t> videoMemory)
{
    if (!m_surface)
    {
        m_logger->Log("Cannot draw without an initialized SDL surface", ELogLevel::ERROR);
        return;
    }

    if (videoMemory.size() < VIDEO_MEMORY_SIZE)
    {
        m_logger->Log("Space Invaders video memory must contain 7168 bytes", ELogLevel::ERROR);
        return;
    }

    const Uint32 black = SDL_MapSurfaceRGB(m_surface, 0, 0, 0);
    const Uint32 white = SDL_MapSurfaceRGB(m_surface, 255, 255, 255);
    const Uint32 green = SDL_MapSurfaceRGB(m_surface, 0, 255, 0);
    const Uint32 red = SDL_MapSurfaceRGB(m_surface, 255, 0, 0);

    if (!SDL_FillSurfaceRect(m_surface, nullptr, black))
    {
        m_logger->Log("Failed to clear surface: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        return;
    }

    for (std::size_t byteIndex = 0; byteIndex < VIDEO_MEMORY_SIZE; byteIndex++)
    {
        const uint8_t pixelByte = videoMemory[byteIndex];
        const int screenX = static_cast<int>(byteIndex / 32);
        const int verticalByte = static_cast<int>(byteIndex % 32);

        for (int bit = 0; bit < 8; bit++)
        {
            if ((pixelByte & (1 << bit)) == 0) continue;

            const int screenY = SCREEN_HEIGHT - 1 - (verticalByte * 8 + bit);
            SDL_Rect pixelRect{screenX * SCALE, screenY * SCALE, SCALE, SCALE};
            Uint32 pixelColor = white;

            if (screenY >= 192)
            {
                pixelColor = green;

                if (screenY >= 240 && (screenX <= 16 || screenX >= SCREEN_WIDTH - 122))
                {
                    pixelColor = white;
                }
            }
            else if (screenY >= 40 && screenY <= 50)
            {
                pixelColor = red;
            }

            if (!SDL_FillSurfaceRect(m_surface, &pixelRect, pixelColor))
            {
                m_logger->Log("Failed to draw pixel: " + std::string(SDL_GetError()), ELogLevel::ERROR);
                return;
            }
        }
    }
}

bool DisplaySDL::Update()
{
    if (!m_window)
    {
        m_logger->Log("Cannot update an uninitialized SDL window", ELogLevel::ERROR);
        return false;
    }

    if (!SDL_UpdateWindowSurface(m_window))
    {
        m_logger->Log("Failed to update window: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        return false;
    }

    return true;
}

void DisplaySDL::Deinit()
{
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        m_surface = nullptr;
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
