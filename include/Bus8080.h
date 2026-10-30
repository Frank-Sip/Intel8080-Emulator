#ifndef BUS8080_H
#define BUS8080_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

class Bus8080
{
public:
    static constexpr std::size_t ADDRESS_SPACE_SIZE = 0x10000;

    Bus8080();
    [[nodiscard]] std::uint8_t Read(std::uint16_t address) const noexcept;
    [[nodiscard]] std::span<const std::uint8_t> ReadRange(std::uint16_t startAddress, std::size_t length) const;
    void Write(std::uint16_t address, std::uint8_t value) noexcept;
    void Load(std::uint16_t startAddress, std::span<const std::uint8_t> data);
    void Reset();

private:
    std::array<std::uint8_t, ADDRESS_SPACE_SIZE> m_memory{};
};

#endif
