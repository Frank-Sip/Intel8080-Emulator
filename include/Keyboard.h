#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>

class Keyboard
{
public:
    virtual ~Keyboard() = default;

    virtual void Init() = 0;
    virtual void Update(uint8_t* inputPorts, bool* running) = 0;
    virtual void Deinit() = 0;
};

#endif
