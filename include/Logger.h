#pragma once

#include <fstream>

namespace Logger
{
    extern std::ofstream g_logFile;

    void initLogger();

    template <typename type>
    void log(const type& arg)
    {
        #if defined DEBUG
        g_logFile << arg;
        #endif
    }

    template <typename type, typename ... args>
    void log(const type& arg, args... other)
    {
        log(arg);
        log(other...);
    }

    void closeLogger();

}

