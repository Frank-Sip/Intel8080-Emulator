#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>

class Keyboard
{
public:
    virtual void Init() = 0;
    virtual void Update(uint8_t* key_map, bool* running) = 0;
    virtual void Deinit() = 0;
};

#endif