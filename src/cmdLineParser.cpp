#include "cmdLineParser.h"
#include <string>

void CMDLineParser::PrintHelp()
{
    std::cout << "Usage: emu [options] <rom_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Print this help message" << std::endl;
    std::cout << "  -r, --rom-directory     Set the rom path to load" << std::endl;
    std::cout << "  -l, --log-level         Set the log level (0: NONE, 1: INFO, 2: WARNING, 3: ERROR, 4: DEBUG)" << std::endl;
}

void CMDLineParser::SetRomFileName(std::string newName)
{
    this->m_romFileName = newName;
}

void CMDLineParser::SetLogLevel(int newLevel)
{
    this -> m_logLevel = newLevel;
}

CMDLineParser::CMDLineParser() : m_romFileName(""), m_logLevel(-1)
{
    m_logger = Logger::GetInstance();
}

void CMDLineParser::Parse(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        std::string aux(argv[i]);

        if (aux == "-h" || aux == "--help")
        {
            this -> PrintHelp();
        }
        else if (aux == "-r" || aux == "--rom-directory")
        {
            i++;
            std::string auxName(argv[i]);
            this -> SetRomFileName(auxName);
        }
        else if (aux == "-l" || aux == "--log-level")
        {
            i++;
            std::string auxLevel(argv[i]);
            if (!std::isdigit(auxLevel.at(0)))
            {
                m_logger->Log("Error: Log level must be a number [0,4]", ELogLevel::ERROR);
                exit(1);
            }
            this -> SetLogLevel(stoi(auxLevel));
        }
        else
        {
            std::string param(argv[i]);
            m_logger->Log("Error: Unknown parameter: " + param, ELogLevel::ERROR);
            exit(1);
        }
    }
}

bool CMDLineParser::IsRomFileNameSet()
{
    return !m_romFileName .empty();
}

std::string CMDLineParser::GetRomFileName()
{
    return this->m_romFileName;
}

bool CMDLineParser::IsLogLevelSet()
{
    return m_logLevel != -1;
}

int CMDLineParser::GetLogLevel()
{
    return m_logLevel;
}