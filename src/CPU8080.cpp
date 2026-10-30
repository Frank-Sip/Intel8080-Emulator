#include "CPU8080.h"

#include <string>

#include "Bus8080.h"

CPU8080::CPU8080(Bus8080* bus) : m_bus(bus)
{
    m_logger = Logger::GetInstance();
    m_inputHandler = [](uint8_t) { return 0; };
    m_outputHandler = [](uint8_t, uint8_t) {};
}

CPU8080::~CPU8080() {}

void CPU8080::Init()
{
    m_Areg = 0;
    m_Breg = 0;
    m_Creg = 0;
    m_Dreg = 0;
    m_Ereg = 0;
    m_Hreg = 0;
    m_Lreg = 0;

    m_pcreg = PC_START;
    m_SPreg = 0;

    m_signFlag = false;
    m_zeroFlag = false;
    m_auxiliaryCarryFlag = false;
    m_parityFlag = false;
    m_carryFlag = false;

    m_current_op = 0;
    m_interruptEnabled = false;
    m_interruptEnableDelay = 0;
    m_halted = false;
    m_cycleCount = 0;
    m_instructionCount = 0;
}

void CPU8080::Fetch()
{
    if (m_halted) return;

    m_current_op = m_bus->Read(m_pcreg);
    m_pcreg++;
}

void CPU8080::Decode() {}

void CPU8080::Execute()
{
    if (m_halted) return;

    m_instructionCount++;

    if (m_current_op >= 0x40 && m_current_op <= 0x7F)
    {
        ExecuteMoveGroup();
        return;
    }

    if (m_current_op >= 0x80 && m_current_op <= 0xBF)
    {
        ExecuteArithmeticGroup();
        return;
    }

    if ((m_current_op & 0xCF) == 0x01)
    {
        WriteRegisterPair((m_current_op >> 4) & 0x03, ReadNextWord());
        CompleteInstruction(10);
        return;
    }

    if ((m_current_op & 0xCF) == 0x03)
    {
        const uint8_t registerPairCode = (m_current_op >> 4) & 0x03;
        WriteRegisterPair(registerPairCode, ReadRegisterPair(registerPairCode) + 1);
        CompleteInstruction(5);
        return;
    }

    if ((m_current_op & 0xC7) == 0x04)
    {
        IncrementRegister((m_current_op >> 3) & 0x07);
        return;
    }

    if ((m_current_op & 0xC7) == 0x05)
    {
        DecrementRegister((m_current_op >> 3) & 0x07);
        return;
    }

    if ((m_current_op & 0xC7) == 0x06)
    {
        const uint8_t registerCode = (m_current_op >> 3) & 0x07;
        WriteRegister(registerCode, ReadNextByte());
        CompleteInstruction(registerCode == 6 ? 10 : 7);
        return;
    }

    if ((m_current_op & 0xCF) == 0x09)
    {
        const uint32_t result = static_cast<uint32_t>(GetHL()) + ReadRegisterPair((m_current_op >> 4) & 0x03);
        m_carryFlag = result > 0xFFFF;
        SetHL(static_cast<uint16_t>(result));
        CompleteInstruction(10);
        return;
    }

    if ((m_current_op & 0xCF) == 0x0B)
    {
        const uint8_t registerPairCode = (m_current_op >> 4) & 0x03;
        WriteRegisterPair(registerPairCode, ReadRegisterPair(registerPairCode) - 1);
        CompleteInstruction(5);
        return;
    }

    if ((m_current_op & 0xC7) == 0xC0)
    {
        if (CheckCondition((m_current_op >> 3) & 0x07))
        {
            m_pcreg = Pop();
            CompleteInstruction(11);
        }
        else
        {
            CompleteInstruction(5);
        }
        return;
    }

    if ((m_current_op & 0xCF) == 0xC1)
    {
        const uint8_t registerPairCode = (m_current_op >> 4) & 0x03;
        const uint16_t value = Pop();

        if (registerPairCode == 3)
        {
            m_Areg = static_cast<uint8_t>(value >> 8);
            UnpackFlags(static_cast<uint8_t>(value));
        }
        else
        {
            WriteRegisterPair(registerPairCode, value);
        }

        CompleteInstruction(10);
        return;
    }

    if ((m_current_op & 0xC7) == 0xC2)
    {
        const uint16_t address = ReadNextWord();
        if (CheckCondition((m_current_op >> 3) & 0x07)) m_pcreg = address;
        CompleteInstruction(10);
        return;
    }

    if ((m_current_op & 0xC7) == 0xC4)
    {
        const uint16_t address = ReadNextWord();

        if (CheckCondition((m_current_op >> 3) & 0x07))
        {
            Push(m_pcreg);
            m_pcreg = address;
            CompleteInstruction(17);
        }
        else
        {
            CompleteInstruction(11);
        }
        return;
    }

    if ((m_current_op & 0xCF) == 0xC5)
    {
        const uint8_t registerPairCode = (m_current_op >> 4) & 0x03;

        if (registerPairCode == 3)
        {
            Push((static_cast<uint16_t>(m_Areg) << 8) | PackFlags());
        }
        else
        {
            Push(ReadRegisterPair(registerPairCode));
        }

        CompleteInstruction(11);
        return;
    }

    if ((m_current_op & 0xC7) == 0xC6)
    {
        const uint8_t value = ReadNextByte();

        switch ((m_current_op >> 3) & 0x07)
        {
            case 0: Add(value, false); break;
            case 1: Add(value, true); break;
            case 2: Subtract(value, false); break;
            case 3: Subtract(value, true); break;
            case 4: And(value); break;
            case 5: Xor(value); break;
            case 6: Or(value); break;
            case 7: Compare(value); break;
        }

        CompleteInstruction(7);
        return;
    }

    if ((m_current_op & 0xC7) == 0xC7)
    {
        Push(m_pcreg);
        m_pcreg = m_current_op & 0x38;
        CompleteInstruction(11);
        return;
    }

    switch (m_current_op)
    {
        case 0x00:
        case 0x08:
        case 0x10:
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
        case 0x38:
            Nop();
            break;

        case 0x02:
            m_bus->Write(GetBC(), m_Areg);
            CompleteInstruction(7);
            break;

        case 0x07:
        {
            const bool bitSeven = (m_Areg & 0x80) != 0;
            m_Areg = static_cast<uint8_t>((m_Areg << 1) | static_cast<uint8_t>(bitSeven));
            m_carryFlag = bitSeven;
            CompleteInstruction(4);
            break;
        }

        case 0x0A:
            m_Areg = m_bus->Read(GetBC());
            CompleteInstruction(7);
            break;

        case 0x0F:
        {
            const bool bitZero = (m_Areg & 0x01) != 0;
            m_Areg = static_cast<uint8_t>((m_Areg >> 1) | (bitZero << 7));
            m_carryFlag = bitZero;
            CompleteInstruction(4);
            break;
        }

        case 0x12:
            m_bus->Write(GetDE(), m_Areg);
            CompleteInstruction(7);
            break;

        case 0x17:
        {
            const bool previousCarry = m_carryFlag;
            m_carryFlag = (m_Areg & 0x80) != 0;
            m_Areg = static_cast<uint8_t>((m_Areg << 1) | static_cast<uint8_t>(previousCarry));
            CompleteInstruction(4);
            break;
        }

        case 0x1A:
            m_Areg = m_bus->Read(GetDE());
            CompleteInstruction(7);
            break;

        case 0x1F:
        {
            const bool previousCarry = m_carryFlag;
            m_carryFlag = (m_Areg & 0x01) != 0;
            m_Areg = static_cast<uint8_t>((m_Areg >> 1) | (previousCarry << 7));
            CompleteInstruction(4);
            break;
        }

        case 0x22:
        {
            const uint16_t address = ReadNextWord();
            WriteWord(address, GetHL());
            CompleteInstruction(16);
            break;
        }

        case 0x27:
            DecimalAdjustAccumulator();
            CompleteInstruction(4);
            break;

        case 0x2A:
        {
            const uint16_t address = ReadNextWord();
            SetHL(ReadWord(address));
            CompleteInstruction(16);
            break;
        }

        case 0x2F:
            m_Areg = static_cast<uint8_t>(~m_Areg);
            CompleteInstruction(4);
            break;

        case 0x32:
            m_bus->Write(ReadNextWord(), m_Areg);
            CompleteInstruction(13);
            break;

        case 0x37:
            m_carryFlag = true;
            CompleteInstruction(4);
            break;

        case 0x3A:
            m_Areg = m_bus->Read(ReadNextWord());
            CompleteInstruction(13);
            break;

        case 0x3F:
            m_carryFlag = !m_carryFlag;
            CompleteInstruction(4);
            break;

        case 0xC3:
        case 0xCB:
            m_pcreg = ReadNextWord();
            CompleteInstruction(10);
            break;

        case 0xC9:
        case 0xD9:
            m_pcreg = Pop();
            CompleteInstruction(10);
            break;

        case 0xCD:
        case 0xDD:
        case 0xED:
        case 0xFD:
        {
            const uint16_t address = ReadNextWord();
            Push(m_pcreg);
            m_pcreg = address;
            CompleteInstruction(17);
            break;
        }

        case 0xD3:
        {
            const uint8_t port = ReadNextByte();
            m_outputHandler(port, m_Areg);
            CompleteInstruction(10);
            break;
        }

        case 0xDB:
        {
            const uint8_t port = ReadNextByte();
            m_Areg = m_inputHandler(port);
            CompleteInstruction(10);
            break;
        }

        case 0xE3:
        {
            const uint8_t stackLow = m_bus->Read(m_SPreg);
            const uint8_t stackHigh = m_bus->Read(static_cast<uint16_t>(m_SPreg + 1));
            m_bus->Write(m_SPreg, m_Lreg);
            m_bus->Write(static_cast<uint16_t>(m_SPreg + 1), m_Hreg);
            m_Lreg = stackLow;
            m_Hreg = stackHigh;
            CompleteInstruction(18);
            break;
        }

        case 0xE9:
            m_pcreg = GetHL();
            CompleteInstruction(5);
            break;

        case 0xEB:
        {
            const uint16_t temporaryValue = GetDE();
            SetDE(GetHL());
            SetHL(temporaryValue);
            CompleteInstruction(4);
            break;
        }

        case 0xF3:
            m_interruptEnabled = false;
            m_interruptEnableDelay = 0;
            CompleteInstruction(4);
            break;

        case 0xF9:
            m_SPreg = GetHL();
            CompleteInstruction(5);
            break;

        case 0xFB:
            m_interruptEnableDelay = 2;
            CompleteInstruction(4);
            break;

        default:
            m_logger->Log("Unimplemented Intel 8080 opcode: " + std::to_string(m_current_op), ELogLevel::ERROR);
            m_halted = true;
            break;
    }
}

void CPU8080::Deinit()
{
}

uint8_t CPU8080::ReadNextByte()
{
    const uint8_t value = m_bus->Read(m_pcreg);
    m_pcreg++;
    return value;
}

uint16_t CPU8080::ReadNextWord()
{
    const uint8_t lowByte = ReadNextByte();
    const uint8_t highByte = ReadNextByte();
    return static_cast<uint16_t>(lowByte | (highByte << 8));
}

uint16_t CPU8080::ReadWord(uint16_t address) const
{
    const uint8_t lowByte = m_bus->Read(address);
    const uint8_t highByte = m_bus->Read(static_cast<uint16_t>(address + 1));
    return static_cast<uint16_t>(lowByte | (highByte << 8));
}

void CPU8080::WriteWord(uint16_t address, uint16_t value)
{
    m_bus->Write(address, static_cast<uint8_t>(value));
    m_bus->Write(static_cast<uint16_t>(address + 1), static_cast<uint8_t>(value >> 8));
}

uint8_t CPU8080::ReadRegister(uint8_t registerCode) const
{
    switch (registerCode)
    {
        case 0: return m_Breg;
        case 1: return m_Creg;
        case 2: return m_Dreg;
        case 3: return m_Ereg;
        case 4: return m_Hreg;
        case 5: return m_Lreg;
        case 6: return m_bus->Read(GetHL());
        case 7: return m_Areg;
        default: return 0;
    }
}

void CPU8080::WriteRegister(uint8_t registerCode, uint8_t value)
{
    switch (registerCode)
    {
        case 0: m_Breg = value; break;
        case 1: m_Creg = value; break;
        case 2: m_Dreg = value; break;
        case 3: m_Ereg = value; break;
        case 4: m_Hreg = value; break;
        case 5: m_Lreg = value; break;
        case 6: m_bus->Write(GetHL(), value); break;
        case 7: m_Areg = value; break;
    }
}

uint16_t CPU8080::ReadRegisterPair(uint8_t registerPairCode) const
{
    switch (registerPairCode)
    {
        case 0: return GetBC();
        case 1: return GetDE();
        case 2: return GetHL();
        case 3: return m_SPreg;
        default: return 0;
    }
}

void CPU8080::WriteRegisterPair(uint8_t registerPairCode, uint16_t value)
{
    switch (registerPairCode)
    {
        case 0: SetBC(value); break;
        case 1: SetDE(value); break;
        case 2: SetHL(value); break;
        case 3: m_SPreg = value; break;
    }
}

uint16_t CPU8080::GetBC() const
{
    return static_cast<uint16_t>((m_Breg << 8) | m_Creg);
}

uint16_t CPU8080::GetDE() const
{
    return static_cast<uint16_t>((m_Dreg << 8) | m_Ereg);
}

uint16_t CPU8080::GetHL() const
{
    return static_cast<uint16_t>((m_Hreg << 8) | m_Lreg);
}

void CPU8080::SetBC(uint16_t value)
{
    m_Breg = static_cast<uint8_t>(value >> 8);
    m_Creg = static_cast<uint8_t>(value);
}

void CPU8080::SetDE(uint16_t value)
{
    m_Dreg = static_cast<uint8_t>(value >> 8);
    m_Ereg = static_cast<uint8_t>(value);
}

void CPU8080::SetHL(uint16_t value)
{
    m_Hreg = static_cast<uint8_t>(value >> 8);
    m_Lreg = static_cast<uint8_t>(value);
}

void CPU8080::Push(uint16_t value)
{
    m_SPreg--;
    m_bus->Write(m_SPreg, static_cast<uint8_t>(value >> 8));
    m_SPreg--;
    m_bus->Write(m_SPreg, static_cast<uint8_t>(value));
}

uint16_t CPU8080::Pop()
{
    const uint8_t lowByte = m_bus->Read(m_SPreg);
    m_SPreg++;
    const uint8_t highByte = m_bus->Read(m_SPreg);
    m_SPreg++;
    return static_cast<uint16_t>(lowByte | (highByte << 8));
}

uint8_t CPU8080::PackFlags() const
{
    uint8_t flags = 0x02;
    if (m_signFlag) flags |= 0x80;
    if (m_zeroFlag) flags |= 0x40;
    if (m_auxiliaryCarryFlag) flags |= 0x10;
    if (m_parityFlag) flags |= 0x04;
    if (m_carryFlag) flags |= 0x01;
    return flags;
}

void CPU8080::UnpackFlags(uint8_t flags)
{
    m_signFlag = (flags & 0x80) != 0;
    m_zeroFlag = (flags & 0x40) != 0;
    m_auxiliaryCarryFlag = (flags & 0x10) != 0;
    m_parityFlag = (flags & 0x04) != 0;
    m_carryFlag = (flags & 0x01) != 0;
}

void CPU8080::SetZeroSignParity(uint8_t value)
{
    m_zeroFlag = value == 0;
    m_signFlag = (value & 0x80) != 0;
    m_parityFlag = HasEvenParity(value);
}

bool CPU8080::HasEvenParity(uint8_t value) const
{
    uint8_t setBitCount = 0;

    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if ((value & (1 << bit)) != 0) setBitCount++;
    }

    return (setBitCount % 2) == 0;
}

bool CPU8080::CheckCondition(uint8_t conditionCode) const
{
    switch (conditionCode)
    {
        case 0: return !m_zeroFlag;
        case 1: return m_zeroFlag;
        case 2: return !m_carryFlag;
        case 3: return m_carryFlag;
        case 4: return !m_parityFlag;
        case 5: return m_parityFlag;
        case 6: return !m_signFlag;
        case 7: return m_signFlag;
        default: return false;
    }
}

void CPU8080::Add(uint8_t value, bool includeCarry)
{
    const uint8_t carry = includeCarry && m_carryFlag ? 1 : 0;
    const uint16_t result = static_cast<uint16_t>(m_Areg) + value + carry;
    m_auxiliaryCarryFlag = ((m_Areg & 0x0F) + (value & 0x0F) + carry) > 0x0F;
    m_carryFlag = result > 0xFF;
    m_Areg = static_cast<uint8_t>(result);
    SetZeroSignParity(m_Areg);
}

void CPU8080::Subtract(uint8_t value, bool includeBorrow)
{
    const uint8_t borrow = includeBorrow && m_carryFlag ? 1 : 0;
    const uint16_t subtrahend = static_cast<uint16_t>(value) + borrow;
    const uint16_t result = static_cast<uint16_t>(m_Areg) - subtrahend;
    m_auxiliaryCarryFlag = (m_Areg & 0x0F) < ((value & 0x0F) + borrow);
    m_carryFlag = static_cast<uint16_t>(m_Areg) < subtrahend;
    m_Areg = static_cast<uint8_t>(result);
    SetZeroSignParity(m_Areg);
}

void CPU8080::Compare(uint8_t value)
{
    const uint16_t result = static_cast<uint16_t>(m_Areg) - value;
    m_auxiliaryCarryFlag = (m_Areg & 0x0F) < (value & 0x0F);
    m_carryFlag = m_Areg < value;
    SetZeroSignParity(static_cast<uint8_t>(result));
}

void CPU8080::And(uint8_t value)
{
    m_auxiliaryCarryFlag = ((m_Areg | value) & 0x08) != 0;
    m_carryFlag = false;
    m_Areg &= value;
    SetZeroSignParity(m_Areg);
}

void CPU8080::Xor(uint8_t value)
{
    m_Areg ^= value;
    m_auxiliaryCarryFlag = false;
    m_carryFlag = false;
    SetZeroSignParity(m_Areg);
}

void CPU8080::Or(uint8_t value)
{
    m_Areg |= value;
    m_auxiliaryCarryFlag = false;
    m_carryFlag = false;
    SetZeroSignParity(m_Areg);
}

void CPU8080::IncrementRegister(uint8_t registerCode)
{
    const uint8_t previousValue = ReadRegister(registerCode);
    const uint8_t result = static_cast<uint8_t>(previousValue + 1);
    m_auxiliaryCarryFlag = (previousValue & 0x0F) == 0x0F;
    WriteRegister(registerCode, result);
    SetZeroSignParity(result);
    CompleteInstruction(registerCode == 6 ? 10 : 5);
}

void CPU8080::DecrementRegister(uint8_t registerCode)
{
    const uint8_t previousValue = ReadRegister(registerCode);
    const uint8_t result = static_cast<uint8_t>(previousValue - 1);
    m_auxiliaryCarryFlag = (previousValue & 0x0F) == 0;
    WriteRegister(registerCode, result);
    SetZeroSignParity(result);
    CompleteInstruction(registerCode == 6 ? 10 : 5);
}

void CPU8080::DecimalAdjustAccumulator()
{
    uint8_t correction = 0;
    bool carry = m_carryFlag;

    if ((m_Areg & 0x0F) > 9 || m_auxiliaryCarryFlag) correction |= 0x06;

    if (m_Areg > 0x99 || m_carryFlag)
    {
        correction |= 0x60;
        carry = true;
    }

    m_auxiliaryCarryFlag = ((m_Areg & 0x0F) + (correction & 0x0F)) > 0x0F;
    m_Areg = static_cast<uint8_t>(m_Areg + correction);
    m_carryFlag = carry;
    SetZeroSignParity(m_Areg);
}

void CPU8080::ExecuteMoveGroup()
{
    if (m_current_op == 0x76)
    {
        Halt();
        return;
    }

    const uint8_t destinationRegister = (m_current_op >> 3) & 0x07;
    const uint8_t sourceRegister = m_current_op & 0x07;
    WriteRegister(destinationRegister, ReadRegister(sourceRegister));
    CompleteInstruction(destinationRegister == 6 || sourceRegister == 6 ? 7 : 5);
}

void CPU8080::ExecuteArithmeticGroup()
{
    const uint8_t sourceRegister = m_current_op & 0x07;
    const uint8_t value = ReadRegister(sourceRegister);

    switch ((m_current_op >> 3) & 0x07)
    {
        case 0: Add(value, false); break;
        case 1: Add(value, true); break;
        case 2: Subtract(value, false); break;
        case 3: Subtract(value, true); break;
        case 4: And(value); break;
        case 5: Xor(value); break;
        case 6: Or(value); break;
        case 7: Compare(value); break;
    }

    CompleteInstruction(sourceRegister == 6 ? 7 : 4);
}

void CPU8080::CompleteInstruction(uint8_t cycles)
{
    m_cycleCount += cycles;

    if (m_interruptEnableDelay > 0)
    {
        m_interruptEnableDelay--;
        if (m_interruptEnableDelay == 0) m_interruptEnabled = true;
    }
}

void CPU8080::Nop()
{
    CompleteInstruction(4);
}

void CPU8080::Halt()
{
    m_halted = true;
    CompleteInstruction(7);
}

void CPU8080::RequestInterrupt(uint8_t interruptNumber)
{
    if (!m_interruptEnabled) return;

    if (interruptNumber > 7)
    {
        m_logger->Log("Invalid Intel 8080 interrupt number: " + std::to_string(interruptNumber), ELogLevel::ERROR);
        return;
    }

    m_interruptEnabled = false;
    m_interruptEnableDelay = 0;
    m_halted = false;
    Push(m_pcreg);
    m_pcreg = static_cast<uint16_t>(interruptNumber * 8);
    m_cycleCount += 11;
}

void CPU8080::SetInputHandler(std::function<uint8_t(uint8_t)> handler)
{
    if (handler) m_inputHandler = handler;
}

void CPU8080::SetOutputHandler(std::function<void(uint8_t, uint8_t)> handler)
{
    if (handler) m_outputHandler = handler;
}

bool CPU8080::IsHalted() const
{
    return m_halted;
}

bool CPU8080::AreInterruptsEnabled() const
{
    return m_interruptEnabled;
}

uint64_t CPU8080::GetCycleCount() const
{
    return m_cycleCount;
}

uint64_t CPU8080::GetInstructionCount() const
{
    return m_instructionCount;
}