#include "Logger.h"
#include "Common.h"

namespace Logger
{

    std::ofstream g_logFile;

    void initLogger()
    {
        #if defined DEBUG
        g_logFile = std::ofstream("logs.txt");
        EVALUATE(g_logFile.is_open(), false, ==, "Unable to open log file");
        log("Log file opened successfully");
        #endif
    }

    void closeLogger()
    {
        #if defined DEBUG
        g_logFile.close();
        #endif
    }

}

