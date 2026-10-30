#include "SpaceInvadersMachine.h"

#include <chrono>
#include <thread>

#include "Bus8080.h"
#include "CPU8080.h"
#include "RomLoader.h"

SpaceInvadersMachine::SpaceInvadersMachine()
{
    m_logger = Logger::GetInstance();
    m_bus = new Bus8080();
    m_cpu = new CPU8080(m_bus);

    m_emulatorRunning = false;
    m_nextInterruptCycle = CYCLES_PER_INTERRUPT;
    m_nextInterruptIsMidScreen = true;

    m_inputPort0 = 0;
    m_inputPort1 = 0;
    m_inputPort2 = 0;
    m_outputPort3 = 0;
    m_outputPort5 = 0;
    m_shiftRegister = 0;
    m_shiftOffset = 0;
}

SpaceInvadersMachine::~SpaceInvadersMachine()
{
    delete m_cpu;
    delete m_bus;
}

void SpaceInvadersMachine::Init(const std::filesystem::path& romDirectory)
{
    m_bus->Reset();
    m_cpu->Init();

    RomLoader loader;
    loader.LoadSpaceInvadersRoms(romDirectory, *m_bus);

    InitPorts();

    m_shiftRegister = 0;
    m_shiftOffset = 0;
    m_nextInterruptCycle = CYCLES_PER_INTERRUPT;
    m_nextInterruptIsMidScreen = true;
    m_emulatorRunning = true;

    m_cpu->SetInputHandler([this](uint8_t port) { return ReadPort(port); });
    m_cpu->SetOutputHandler([this](uint8_t port, uint8_t value) { WritePort(port, value); });

    m_logger->Log("Space Invaders machine initialized", ELogLevel::INFO);
}

void SpaceInvadersMachine::Run()
{
    using clock = std::chrono::steady_clock;
    const std::chrono::nanoseconds interruptInterval(1000000000 / 120);
    clock::time_point nextInterruptTime = clock::now() + interruptInterval;

    while (m_emulatorRunning)
    {
        if (!m_cpu->IsHalted())
        {
            m_cpu->Fetch();
            m_cpu->Decode();
            m_cpu->Execute();
        }

        if (m_cpu->IsHalted() || m_cpu->GetCycleCount() >= m_nextInterruptCycle)
        {
            std::this_thread::sleep_until(nextInterruptTime);
            GenerateInterrupt();

            m_nextInterruptCycle += CYCLES_PER_INTERRUPT;
            nextInterruptTime += interruptInterval;
        }
    }
}

void SpaceInvadersMachine::Deinit()
{
    m_emulatorRunning = false;
    m_cpu->Deinit();
    m_logger->Log("Space Invaders machine deinitialized", ELogLevel::INFO);
}

void SpaceInvadersMachine::Stop()
{
    m_emulatorRunning = false;
}

void SpaceInvadersMachine::InitPorts()
{
    m_inputPort0 = 0x0E;
    m_inputPort1 = 0x08;
    m_inputPort2 = 0x00;
    m_outputPort3 = 0;
    m_outputPort5 = 0;
}

uint8_t SpaceInvadersMachine::ReadPort(uint8_t port)
{
    switch (port)
    {
        case 0: return m_inputPort0;
        case 1: return m_inputPort1;
        case 2: return m_inputPort2;
        case 3: return static_cast<uint8_t>(m_shiftRegister >> (8 - m_shiftOffset));
        default: return 0;
    }
}

void SpaceInvadersMachine::WritePort(uint8_t port, uint8_t value)
{
    switch (port)
    {
        case 2:
            m_shiftOffset = value & 0x07;
            break;

        case 3:
            m_outputPort3 = value;
            break;

        case 4:
            m_shiftRegister = static_cast<uint16_t>((value << 8) | (m_shiftRegister >> 8));
            break;

        case 5:
            m_outputPort5 = value;
            break;

        case 6:
            break;
    }
}

void SpaceInvadersMachine::GenerateInterrupt()
{
    if (m_nextInterruptIsMidScreen)
    {
        m_cpu->RequestInterrupt(1);
    }
    else
    {
        m_cpu->RequestInterrupt(2);
    }

    m_nextInterruptIsMidScreen = !m_nextInterruptIsMidScreen;
}
