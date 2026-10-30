#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

class Display
{
public:
    virtual void Init() = 0;
    virtual void Draw(uint8_t framebuffer[][64], uint8_t width, uint8_t height) = 0;
    virtual void Update() = 0;
    virtual void Deinit() = 0;
};

#endif
