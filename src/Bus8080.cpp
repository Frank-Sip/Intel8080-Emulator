#include "Bus8080.h"

#include <algorithm>
#include <stdexcept>

Bus8080::Bus8080()
{
    Reset();
}

void Bus8080::Reset()
{
    m_memory.fill(0);
}

std::uint8_t Bus8080::Read(const std::uint16_t address) const noexcept
{
    return m_memory[address];
}

std::span<const std::uint8_t> Bus8080::ReadRange(const std::uint16_t startAddress, const std::size_t length) const
{
    const std::size_t start = startAddress;

    if (length > m_memory.size() - start)
    {
        throw std::out_of_range("Memory range exceeds the Intel 8080 address space");
    }

    return std::span<const std::uint8_t>(m_memory.data() + start, length);
}

void Bus8080::Write(
    const std::uint16_t address,
    const std::uint8_t value) noexcept
{
    m_memory[address] = value;
}

void Bus8080::Load(const std::uint16_t startAddress, const std::span<const std::uint8_t> data)
{
    const std::size_t start = startAddress;
    
    if (data.size() > m_memory.size() - start)
    {
        throw std::out_of_range("ROM data exceeds the Intel 8080 address space");
    }

    std::copy(data.begin(), data.end(), m_memory.begin() + start);
}
