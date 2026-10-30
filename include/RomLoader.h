#ifndef ROM_LOADER_H
#define ROM_LOADER_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "Logger.h"

class Bus8080;

class RomLoader
{
    std::shared_ptr<Logger> m_logger;

public:
    RomLoader();
    void LoadRom(const std::filesystem::path& filePath, Bus8080& bus, std::uint16_t startAddress, std::size_t expectedSize = 0);
    void LoadSpaceInvadersRoms(const std::filesystem::path& romDirectory, Bus8080& bus);

private:
    std::vector<std::uint8_t> ReadFile(const std::filesystem::path& filePath) const;
};

#endif
