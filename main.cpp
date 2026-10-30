#include <exception>
#include <string>

#include "cmdLineParser.h"
#include "Logger.h"
#include "SDLDisplay.h"
#include "SpaceInvadersMachine.h"

int main(int argc, char** argv)
{
    std::shared_ptr<Logger> logger = Logger::GetInstance();
    CMDLineParser parser;
    parser.Parse(argc, argv);

    logger->Log("Starting Space Invaders emulator", ELogLevel::INFO);

    if (parser.IsLogLevelSet())
    {
        switch (parser.GetLogLevel())
        {
            case 0: logger->SetLogLevel(ELogLevel::NONE); break;
            case 1: logger->SetLogLevel(ELogLevel::INFO); break;
            case 2: logger->SetLogLevel(ELogLevel::WARNING); break;
            case 3: logger->SetLogLevel(ELogLevel::ERROR); break;
            case 4: logger->SetLogLevel(ELogLevel::DEBUG); break;
            default:
                logger->Log("Log level must be between 0 and 4", ELogLevel::ERROR);
                return 1;
        }
    }

    if (!parser.IsRomFileNameSet())
    {
        logger->Log("A ROM file or directory must be provided with -r", ELogLevel::ERROR);
        return 1;
    }

    DisplaySDL display;
    SpaceInvadersMachine machine;
    machine.SetDisplay(&display);

    try
    {
        machine.Init(parser.GetRomFileName());
        machine.Run();
        machine.Deinit();
    }
    catch (const std::exception& exception)
    {
        logger->Log("Emulator error: " + std::string(exception.what()), ELogLevel::ERROR);
        machine.Deinit();
        return 1;
    }

    return 0;
}
