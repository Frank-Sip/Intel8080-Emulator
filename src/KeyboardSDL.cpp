#include <cstdint>
#include <SDL3/SDL.h>

#include "KeyboardSDL.h"

KeyboardSDL::KeyboardSDL()
{
    m_logger = Logger::GetInstance();
}

KeyboardSDL::~KeyboardSDL() {}

void KeyboardSDL::Init() {}

void KeyboardSDL::Update(uint8_t* key_map, bool* running)
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            switch (event.key.key)
            {
                case SDLK_1: key_map[0x1] = 1; break;
                case SDLK_2: key_map[0x2] = 1; break;
                case SDLK_3: key_map[0x3] = 1; break;
                case SDLK_4: key_map[0xC] = 1; break;
                case SDLK_Q: key_map[0x4] = 1; break;
                case SDLK_W: key_map[0x5] = 1; break;
                case SDLK_E: key_map[0x6] = 1; break;
                case SDLK_R: key_map[0xD] = 1; break;
                case SDLK_A: key_map[0x7] = 1; break;
                case SDLK_S: key_map[0x8] = 1; break;
                case SDLK_D: key_map[0x9] = 1; break;
                case SDLK_F: key_map[0xE] = 1; break;
                case SDLK_Z: key_map[0xA] = 1; break;
                case SDLK_X: key_map[0x0] = 1; break;
                case SDLK_C: key_map[0xB] = 1; break;
                case SDLK_V: key_map[0xF] = 1; break;
            }
        }
        else if (event.type == SDL_EVENT_KEY_UP)
        {
            switch (event.key.key)
            {
                case SDLK_1: key_map[0x1] = 0; break;
                case SDLK_2: key_map[0x2] = 0; break;
                case SDLK_3: key_map[0x3] = 0; break;
                case SDLK_4: key_map[0xC] = 0; break;
                case SDLK_Q: key_map[0x4] = 0; break;
                case SDLK_W: key_map[0x5] = 0; break;
                case SDLK_E: key_map[0x6] = 0; break;
                case SDLK_R: key_map[0xD] = 0; break;
                case SDLK_A: key_map[0x7] = 0; break;
                case SDLK_S: key_map[0x8] = 0; break;
                case SDLK_D: key_map[0x9] = 0; break;
                case SDLK_F: key_map[0xE] = 0; break;
                case SDLK_Z: key_map[0xA] = 0; break;
                case SDLK_X: key_map[0x0] = 0; break;
                case SDLK_C: key_map[0xB] = 0; break;
                case SDLK_V: key_map[0xF] = 0; break;
            }
        }
        else if (event.type == SDL_EVENT_QUIT)
        {
            *running = false;
        }
    }
}

void KeyboardSDL::Deinit() {}
