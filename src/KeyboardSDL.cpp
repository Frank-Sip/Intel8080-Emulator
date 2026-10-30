#include <cstdint>
#include <SDL3/SDL.h>

#include "KeyboardSDL.h"

KeyboardSDL::KeyboardSDL()
{
    m_logger = Logger::GetInstance();
}

KeyboardSDL::~KeyboardSDL() {}

void KeyboardSDL::Init() {}

void KeyboardSDL::Update(uint8_t* inputPorts, bool* running)
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            *running = false;
            continue;
        }

        if (event.type != SDL_EVENT_KEY_DOWN && event.type != SDL_EVENT_KEY_UP) continue;

        const bool pressed = event.type == SDL_EVENT_KEY_DOWN;

        const auto setInputBit = [inputPorts, pressed](uint8_t port, uint8_t bit)
        {
            const uint8_t mask = static_cast<uint8_t>(1u << bit);

            if (pressed)
            {
                inputPorts[port] |= mask;
            }
            else
            {
                inputPorts[port] &= static_cast<uint8_t>(~mask);
            }
        };

        switch (event.key.key)
        {
            case SDLK_RETURN:
                setInputBit(1, 0);
                break;

            case SDLK_1:
                setInputBit(1, 2);
                break;

            case SDLK_2:
                setInputBit(1, 1);
                break;

            case SDLK_SPACE:
                setInputBit(0, 4);
                setInputBit(1, 4);
                setInputBit(2, 4);
                break;

            case SDLK_A:
            case SDLK_LEFT:
                setInputBit(0, 5);
                setInputBit(1, 5);
                setInputBit(2, 5);
                break;

            case SDLK_D:
            case SDLK_RIGHT:
                setInputBit(0, 6);
                setInputBit(1, 6);
                setInputBit(2, 6);
                break;

            case SDLK_DELETE:
                setInputBit(2, 2);
                break;
        }
    }
}

void KeyboardSDL::Deinit() {}
