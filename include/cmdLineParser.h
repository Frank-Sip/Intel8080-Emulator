#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

#include "Logger.h"

class CMDLineParser
{
    std::shared_ptr<Logger> m_logger;

    std::string m_romFileName;
    int m_logLevel;

private:
    void PrintHelp();
    void SetRomFileName(std::string newName);
    void SetLogLevel(int newLevel);

public:
    CMDLineParser();
    void Parse(int argc, char** argv);
    bool IsRomFileNameSet();
    std::string GetRomFileName();
    bool IsLogLevelSet();
    int GetLogLevel();
};
#endif