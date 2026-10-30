#include "Logger.h"

int main(int argc, char** argv)
{
    std::shared_ptr<Logger> logger = Logger::GetInstance();

    logger->Log("Starting Space Invaders emulator", ELogLevel::INFO);
    logger->Log("Starting Space Invaders emulator", ELogLevel::ERROR);
    logger->Log("Starting Space Invaders emulator", ELogLevel::WARNING);
    logger->Log("Starting Space Invaders emulator", ELogLevel::DEBUG);

    return 0;
}