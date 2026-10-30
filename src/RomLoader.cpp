#include "RomLoader.h"

#include <array>
#include <fstream>
#include <stdexcept>
#include <string>

#include "Bus8080.h"

namespace
{
struct RomDefinition
{
    const char* fileName;
    std::uint16_t startAddress;
};

constexpr std::size_t ROM_CHIP_SIZE = 0x0800;

constexpr std::array<RomDefinition, 4> SPACE_INVADERS_ROMS{{{"invaders.h", 0x0000}, {"invaders.g", 0x0800}, {"invaders.f", 0x1000}, {"invaders.e", 0x1800},}};
}

RomLoader::RomLoader() : m_logger(Logger::GetInstance()) {}

void RomLoader::LoadRom(const std::filesystem::path& filePath, Bus8080& bus, const std::uint16_t startAddress, const std::size_t expectedSize)
{
    const std::string pathText = filePath.string();
    m_logger->Log("Loading ROM from path: " + pathText, ELogLevel::INFO);

    const std::vector<std::uint8_t> data = ReadFile(filePath);

    if (expectedSize != 0 && data.size() != expectedSize)
    {
        const std::string message = "Unexpected ROM size for " + pathText + ": expected " + std::to_string(expectedSize) + " bytes, got " + std::to_string(data.size()) + " bytes";
        m_logger->Log(message, ELogLevel::ERROR);
        throw std::runtime_error(message);
    }

    bus.Load(startAddress, data);

    m_logger->Log("Successfully loaded ROM: " + std::to_string(data.size()) + " bytes at address " + std::to_string(startAddress), ELogLevel::INFO);
}

void RomLoader::LoadSpaceInvadersRoms(const std::filesystem::path& romDirectory, Bus8080& bus)
{
    for (const RomDefinition& rom : SPACE_INVADERS_ROMS)
    {
        LoadRom(romDirectory / rom.fileName, bus, rom.startAddress, ROM_CHIP_SIZE);
    }
}

std::vector<std::uint8_t> RomLoader::ReadFile(const std::filesystem::path& filePath) const
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file)
    {
        const std::string message = "Failed to open ROM file: " + filePath.string();
        m_logger->Log(message, ELogLevel::ERROR);
        throw std::runtime_error(message);
    }

    const std::streampos endPosition = file.tellg();
    if (endPosition < 0)
    {
        const std::string message = "Failed to determine ROM size: " + filePath.string();
        m_logger->Log(message, ELogLevel::ERROR);
        throw std::runtime_error(message);
    }

    const auto size = static_cast<std::size_t>(endPosition);
    std::vector<std::uint8_t> data(size);

    file.seekg(0, std::ios::beg);
    if (size != 0 && !file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size)))
    {
        const std::string message = "Failed to read ROM file: " + filePath.string();
        m_logger->Log(message, ELogLevel::ERROR);
        throw std::runtime_error(message);
    }

    return data;
}
