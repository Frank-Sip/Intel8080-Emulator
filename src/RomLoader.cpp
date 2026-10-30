#include "RomLoader.h"

RomLoader::RomLoader()
{
    m_logger = Logger::GetInstance();
}

RomLoader::~RomLoader() {}

void RomLoader::LoadRom(std::string& path, uint8_t* memory)
{
    m_logger->Log("Loading ROM from path: " + path, ELogLevel::INFO);

    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        m_logger->Log("Failed to open ROM file: " + path, ELogLevel::ERROR);
        exit(1);
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file.read(reinterpret_cast<char*>(memory), size))
    {
        m_logger->Log("Successfully loaded ROM: " + std::to_string(size) + " bytes", ELogLevel::INFO);
    }
    else
    {
        m_logger->Log("Failed to read ROM file: " + path, ELogLevel::ERROR);
        exit(1);
    }

    file.close();
}