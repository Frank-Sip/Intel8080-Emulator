#ifndef KEYBOARDSDL_H
#define KEYBOARDSDL_H

#include <cstdint>

#include "Keyboard.h"
#include "Logger.h"

class KeyboardSDL : public Keyboard
{
    std::shared_ptr<Logger> m_logger;

public:
    KeyboardSDL();
    ~KeyboardSDL();

    void Init() override;
    void Update(uint8_t* key_map, bool* running) override;
    void Deinit() override;
};

#endif