#include "Logger.h"


namespace Logger
{

    std::ofstream g_logFile;

    void initLogger()
    {
        #if defined DEBUG
        g_logFile = std::ofstream("logs.txt");
        #endif
    }

    void closeLogger()
    {
        #if defined DEBUG
        g_logFile.close();
        #endif
    }

}

