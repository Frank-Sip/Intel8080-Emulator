#ifndef CPU8080_H
#define CPU8080_H

#include <cstdint>
#include <functional>
#include <memory>

#include "Logger.h"

#define PC_START 0x0000

class Bus8080;

class CPU8080
{
    // Registers
    uint8_t m_Areg;
    uint8_t m_Breg;
    uint8_t m_Creg;
    uint8_t m_Dreg;
    uint8_t m_Ereg;
    uint8_t m_Hreg;
    uint8_t m_Lreg;

    uint16_t m_pcreg;
    uint16_t m_SPreg;

    // Flags
    bool m_signFlag;
    bool m_zeroFlag;
    bool m_auxiliaryCarryFlag;
    bool m_parityFlag;
    bool m_carryFlag;

    // Helper variables
    uint8_t m_current_op;
    bool m_interruptEnabled;
    uint8_t m_interruptEnableDelay;
    bool m_halted;
    uint64_t m_cycleCount;
    uint64_t m_instructionCount;

    // Bus
    Bus8080* m_bus;

    // I/O communication
    std::function<uint8_t(uint8_t)> m_inputHandler;
    std::function<void(uint8_t, uint8_t)> m_outputHandler;

    // Logging
    std::shared_ptr<Logger> m_logger;

    // Instructions implementation
    uint8_t ReadNextByte();
    uint16_t ReadNextWord();
    uint16_t ReadWord(uint16_t address) const;
    void WriteWord(uint16_t address, uint16_t value);

    uint8_t ReadRegister(uint8_t registerCode) const;
    void WriteRegister(uint8_t registerCode, uint8_t value);
    uint16_t ReadRegisterPair(uint8_t registerPairCode) const;
    void WriteRegisterPair(uint8_t registerPairCode, uint16_t value);
    uint16_t GetBC() const;
    uint16_t GetDE() const;
    uint16_t GetHL() const;
    void SetBC(uint16_t value);
    void SetDE(uint16_t value);
    void SetHL(uint16_t value);

    void Push(uint16_t value);
    uint16_t Pop();
    uint8_t PackFlags() const;
    void UnpackFlags(uint8_t flags);

    void SetZeroSignParity(uint8_t value);
    bool HasEvenParity(uint8_t value) const;
    bool CheckCondition(uint8_t conditionCode) const;
    void Add(uint8_t value, bool includeCarry);
    void Subtract(uint8_t value, bool includeBorrow);
    void Compare(uint8_t value);
    void And(uint8_t value);
    void Xor(uint8_t value);
    void Or(uint8_t value);
    void IncrementRegister(uint8_t registerCode);
    void DecrementRegister(uint8_t registerCode);
    void DecimalAdjustAccumulator();

    void ExecuteMoveGroup();
    void ExecuteArithmeticGroup();
    void CompleteInstruction(uint8_t cycles);
    void Nop();
    void Halt();

public:
    CPU8080(Bus8080* bus);
    ~CPU8080();

    void Init();
    void Fetch();
    void Decode();
    void Execute();
    void Deinit();

    void RequestInterrupt(uint8_t interruptNumber);
    void SetInputHandler(std::function<uint8_t(uint8_t)> handler);
    void SetOutputHandler(std::function<void(uint8_t, uint8_t)> handler);

    bool IsHalted() const;
    bool AreInterruptsEnabled() const;
    uint64_t GetCycleCount() const;
    uint64_t GetInstructionCount() const;
};

#endif