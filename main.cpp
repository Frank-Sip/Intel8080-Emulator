#include "cmdLineParser.h"
#include "KeyboardSDL.h"
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
            default: logger->Log("Log level must be between 0 and 4", ELogLevel::ERROR); return 1;
        }
    }

    if (!parser.IsRomFileNameSet())
    {
        logger->Log("A ROM file or directory must be provided with -r", ELogLevel::ERROR);
        return 1;
    }

    DisplaySDL display;
    KeyboardSDL keyboard;
    SpaceInvadersMachine machine;
    machine.SetDisplay(&display);
    machine.SetKeyboard(&keyboard);
    machine.Init(parser.GetRomFileName());
    machine.Run();
    machine.Deinit();

    return 0;
}