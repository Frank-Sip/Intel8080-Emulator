#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <memory>

enum class ELogLevel
{
    NONE,
    INFO,
    ERROR,
    WARNING,
    DEBUG
};

class Logger
{
    ELogLevel m_logLevel;
    static std::shared_ptr<Logger> m_instance;

public:
    Logger();
    void SetLogLevel(ELogLevel level);
    void Log(std::string message, ELogLevel level);
    static std::shared_ptr<Logger> GetInstance();
};

#endif