#ifndef SDL_DISPLAY_H
#define SDL_DISPLAY_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <SDL3/SDL.h>

#include "Display.h"
#include "Logger.h"

class DisplaySDL : public Display
{
    static constexpr int SCREEN_WIDTH = 224;
    static constexpr int SCREEN_HEIGHT = 256;
    static constexpr int SCALE = 3;
    static constexpr std::size_t VIDEO_MEMORY_SIZE = 0x1C00;

    SDL_Window* m_window;
    SDL_Surface* m_surface;

    std::shared_ptr<Logger> m_logger;

public:
    DisplaySDL();
    ~DisplaySDL() override;

    void Init() override;
    void Draw(std::span<const uint8_t> videoMemory) override;
    bool Update() override;
    void Deinit() override;
};

#endif
