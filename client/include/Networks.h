#pragma once


#include "Common.h"
#include "Logger.h"


inline int ReadMessage(Socket clientSocket, std::string& message)
{
    int size;
    int readResult = read(clientSocket, &size, sizeof(size));
    if (readResult < sizeof(int))
    {
        Logger::log("Attempt to read a message size resulted in a read() with value ", readResult, "\n");
        return -1;
    }
    message.resize(size);
    readResult = read(clientSocket, (void*)message.c_str(), size);
    if (readResult < size)
    {
        Logger::log("Attempt to read a message resulted in a read() with value ", readResult, "\n");
        return -1;
    }
    Logger::log("Recevied command: \"", message, "\" with length = ", size, "\n");

    return 0;
}


inline int SendMessage(Socket clientSocket, std::string message)
{
    int size = message.size();
    Logger::log("Sending command with size = ", size, "\n");
    int writeResult = write(clientSocket, &size, sizeof(size));
    if (writeResult < sizeof(int))
    {
        Logger::log("Attempt to write message resulted in a write() with value ", writeResult, "\n");
        return -1;
    }

    if (size > 0)
    {
        Logger::log("Sending command: \"", message, "\"\n");
        writeResult = write(clientSocket, message.c_str(), size);
        if (writeResult < size)
        {
            Logger::log("Attempt to write message resulted in a write() with value ", writeResult, "\n");
            return -1;
        }
    }

    return 0;
}
