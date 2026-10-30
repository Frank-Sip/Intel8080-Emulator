#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <span>

class Display
{
public:
    virtual ~Display() = default;

    virtual void Init() = 0;
    virtual void Draw(std::span<const uint8_t> videoMemory) = 0;
    virtual bool Update() = 0;
    virtual void Deinit() = 0;
};

#endif
