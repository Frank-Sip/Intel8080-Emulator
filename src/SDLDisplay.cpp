#include "SDLDisplay.h"

#include <cstdlib>
#include <string>

DisplaySDL::DisplaySDL()
    : m_window(nullptr),
      m_surface(nullptr),
      m_logger(Logger::GetInstance())
{
}

DisplaySDL::~DisplaySDL() = default;

void DisplaySDL::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        m_logger->Log("SDL initialization failed: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        std::exit(1);
    }

    constexpr int zoomFactor = 10;

    m_window = SDL_CreateWindow("CHIP-8 Emulator", 64 * zoomFactor, 32 * zoomFactor, 0);

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

void DisplaySDL::Draw(uint8_t framebuffer[][64], uint8_t width, uint8_t height)
{
    constexpr int zoomFactor = 10;

    const Uint32 black = SDL_MapSurfaceRGB(m_surface, 0, 0, 0);
    const Uint32 green = SDL_MapSurfaceRGB(m_surface, 0, 255, 0);

    if (!SDL_FillSurfaceRect(m_surface, nullptr, black))
    {
        m_logger->Log("Failed to clear surface: " + std::string(SDL_GetError()), ELogLevel::ERROR);
        return;
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (framebuffer[y][x] == 0)
            {
                continue;
            }

            SDL_Rect pixelRect{x * zoomFactor, y * zoomFactor, zoomFactor, zoomFactor};

            SDL_FillSurfaceRect(m_surface, &pixelRect, green);
        }
    }
}

void DisplaySDL::Update()
{
    if (!SDL_UpdateWindowSurface(m_window))
    {
        m_logger->Log("Failed to update window: " + std::string(SDL_GetError()), ELogLevel::ERROR);
    }

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            m_logger->Log("Closing window", ELogLevel::INFO);
        }
    }
}

void DisplaySDL::Deinit()
{
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        m_surface = nullptr;
    }

    SDL_Quit();
}
