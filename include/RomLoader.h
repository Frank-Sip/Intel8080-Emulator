#ifndef ROM_LOADER_H
#define ROM_LOADER_H

#include <string>
#include "Logger.h"

class RomLoader
{
    std::shared_ptr<Logger> m_logger;

public:
    RomLoader();
    ~RomLoader();

    void LoadRom(std::string& path, uint8_t* memory);
};

#endif