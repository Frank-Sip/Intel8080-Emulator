#include "SpaceInvadersMachine.h"

#include <chrono>
#include <thread>

#include "Bus8080.h"
#include "CPU8080.h"
#include "Display.h"
#include "Keyboard.h"
#include "RomLoader.h"
#include "Sound.h"

SpaceInvadersMachine::SpaceInvadersMachine()
{
    m_logger = Logger::GetInstance();
    m_bus = new Bus8080();
    m_cpu = new CPU8080(m_bus);
    m_display = nullptr;
    m_keyboard = nullptr;
    m_sound = nullptr;

    m_emulatorRunning = false;
    m_nextInterruptCycle = CYCLES_PER_INTERRUPT;
    m_nextInterruptIsMidScreen = true;

    m_inputPorts[0] = 0;
    m_inputPorts[1] = 0;
    m_inputPorts[2] = 0;
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

void SpaceInvadersMachine::Init(const std::filesystem::path& romPath)
{
    m_bus->Reset();
    m_cpu->Init();

    RomLoader loader;

    if (std::filesystem::is_regular_file(romPath))
    {
        loader.LoadRom(romPath, *m_bus, 0x0000, 0x2000);
    }
    else
    {
        loader.LoadSpaceInvadersRoms(romPath, *m_bus);
    }

    InitPorts();

    m_shiftRegister = 0;
    m_shiftOffset = 0;
    m_nextInterruptCycle = CYCLES_PER_INTERRUPT;
    m_nextInterruptIsMidScreen = true;
    m_emulatorRunning = true;

    m_cpu->SetInputHandler([this](uint8_t port) { return ReadPort(port); });
    m_cpu->SetOutputHandler([this](uint8_t port, uint8_t value) { WritePort(port, value); });

    if (m_display) m_display->Init();
    if (m_keyboard) m_keyboard->Init();
    if (m_sound) m_sound->Init();

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
    if (m_sound) m_sound->Deinit();
    if (m_keyboard) m_keyboard->Deinit();
    if (m_display) m_display->Deinit();
    m_logger->Log("Space Invaders machine deinitialized", ELogLevel::INFO);
}

void SpaceInvadersMachine::Stop()
{
    m_emulatorRunning = false;
}

void SpaceInvadersMachine::SetDisplay(Display* display)
{
    m_display = display;
}

void SpaceInvadersMachine::SetKeyboard(Keyboard* keyboard)
{
    m_keyboard = keyboard;
}

void SpaceInvadersMachine::SetSound(Sound* sound)
{
    m_sound = sound;
}

void SpaceInvadersMachine::InitPorts()
{
    m_inputPorts[0] = 0x0E;
    m_inputPorts[1] = 0x08;
    m_inputPorts[2] = 0x00;
    m_outputPort3 = 0;
    m_outputPort5 = 0;
}

uint8_t SpaceInvadersMachine::ReadPort(uint8_t port)
{
    switch (port)
    {
        case 0:
        case 1:
        case 2:
            return m_inputPorts[port];

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
        {
            const uint8_t risingEdges = static_cast<uint8_t>(value & ~m_outputPort3);
            const uint8_t fallingEdges = static_cast<uint8_t>(~value & m_outputPort3);

            if (m_sound)
            {
                if ((risingEdges & (1 << 0)) != 0) m_sound->PlaySound(0);
                if ((fallingEdges & (1 << 0)) != 0) m_sound->StopSound(0);
                if ((risingEdges & (1 << 1)) != 0) m_sound->PlaySound(1);
                if ((risingEdges & (1 << 2)) != 0) m_sound->PlaySound(2);
                if ((risingEdges & (1 << 3)) != 0) m_sound->PlaySound(3);
            }

            m_outputPort3 = value;
            break;
        }

        case 4:
            m_shiftRegister = static_cast<uint16_t>((value << 8) | (m_shiftRegister >> 8));
            break;

        case 5:
        {
            const uint8_t risingEdges = static_cast<uint8_t>(value & ~m_outputPort5);

            if (m_sound)
            {
                if ((risingEdges & (1 << 0)) != 0) m_sound->PlaySound(4);
                if ((risingEdges & (1 << 1)) != 0) m_sound->PlaySound(5);
                if ((risingEdges & (1 << 2)) != 0) m_sound->PlaySound(6);
                if ((risingEdges & (1 << 3)) != 0) m_sound->PlaySound(7);
            }

            m_outputPort5 = value;
            break;
        }

        case 6:
            break;
    }
}

void SpaceInvadersMachine::GenerateInterrupt()
{
    if (m_keyboard)
    {
        m_keyboard->Update(m_inputPorts, &m_emulatorRunning);
        if (!m_emulatorRunning) return;
    }

    if (m_sound) m_sound->Update();

    if (m_nextInterruptIsMidScreen)
    {
        m_cpu->RequestInterrupt(1);
    }
    else
    {
        m_cpu->RequestInterrupt(2);

        if (m_display)
        {
            m_display->Draw(m_bus->ReadRange(VIDEO_MEMORY_START, VIDEO_MEMORY_SIZE));
            if (!m_display->Update()) Stop();
        }
    }

    m_nextInterruptIsMidScreen = !m_nextInterruptIsMidScreen;
}
