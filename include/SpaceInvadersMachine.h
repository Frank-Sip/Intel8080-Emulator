#ifndef SPACE_INVADERS_MACHINE_H
#define SPACE_INVADERS_MACHINE_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

#include "Logger.h"

class Bus8080;
class CPU8080;
class Display;

class SpaceInvadersMachine
{
    static constexpr uint64_t CYCLES_PER_INTERRUPT = 16640;
    static constexpr uint16_t VIDEO_MEMORY_START = 0x2400;
    static constexpr std::size_t VIDEO_MEMORY_SIZE = 0x1C00;

    //Core components
    Bus8080* m_bus;
    CPU8080* m_cpu;
    Display* m_display;

    //Machine state
    bool m_emulatorRunning;
    uint64_t m_nextInterruptCycle;
    bool m_nextInterruptIsMidScreen;

    //Input ports
    uint8_t m_inputPort0;
    uint8_t m_inputPort1;
    uint8_t m_inputPort2;

    //Output ports
    uint8_t m_outputPort3;
    uint8_t m_outputPort5;

    //Space Invaders shift hardware
    uint16_t m_shiftRegister;
    uint8_t m_shiftOffset;

    std::shared_ptr<Logger> m_logger;

    void InitPorts();
    uint8_t ReadPort(uint8_t port);
    void WritePort(uint8_t port, uint8_t value);
    void GenerateInterrupt();

public:
    SpaceInvadersMachine();
    ~SpaceInvadersMachine();

    void Init(const std::filesystem::path& romPath);
    void Run();
    void Deinit();
    void Stop();
    void SetDisplay(Display* display);
};

#endif
