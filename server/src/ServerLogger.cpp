#include "ServerLogger.h"
#include "Common.h"

namespace Logger
{

    std::ofstream g_logFile;

    void initLogger(const std::string& filename)
    {
        #if defined DEBUG
        g_logFile = std::ofstream(filename.c_str());
        EVALUATE(g_logFile.is_open(), false, ==, "Unable to open log file");
        log("Log file opened successfully\n");
        #endif
    }

    void closeLogger()
    {
        #if defined DEBUG
        g_logFile.close();
        #endif
    }

}

