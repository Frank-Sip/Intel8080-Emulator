#include <iostream>
#include "Logger.h"

std::shared_ptr<Logger> Logger::m_instance = nullptr;

Logger::Logger() : m_logLevel(ELogLevel::DEBUG) {}

std::shared_ptr<Logger> Logger::GetInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = std::make_shared<Logger>();
    }

    return m_instance;
}

void Logger::SetLogLevel(ELogLevel level)
{
    m_logLevel = level;
}

void Logger::Log(std::string message, ELogLevel level)
{
    std::string buf = "";

    if (level > m_logLevel) return;

    switch (level)
    {
        case ELogLevel::INFO:
            buf += "\033[32m[INFO] ";
            break;
        case ELogLevel::ERROR:
            buf += "\033[31m[ERROR] ";
            break;
        case ELogLevel::WARNING:
            buf += "\033[33m[WARNING] ";
            break;
        case ELogLevel::DEBUG:
            buf += "\033[0m[DEBUG] ";
            break;
        case ELogLevel::NONE: return;
        default:
            buf += "\033[0m[UNKNOWN] ";
            break;
    }

    buf += message + "\033[0m\n";
    std::cout << buf;
}