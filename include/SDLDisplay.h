#ifndef DISPLAYSDL_H
#define DISPLAYSDL_H

#include <cstdint>
#include <SDL3/SDL.h>

#include "Display.h"
#include "Logger.h"

class DisplaySDL : public Display
{
    SDL_Window* m_window;
    SDL_Surface* m_surface;

    std::shared_ptr<Logger> m_logger;

public:
    DisplaySDL();
    ~DisplaySDL();

    void Init() override;
    void Draw(uint8_t framebuffer[][64], uint8_t width, uint8_t height) override;
    void Update() override;
    void Deinit() override;
};

#endif
