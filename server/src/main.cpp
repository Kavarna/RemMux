#include "Common.h"
#include "ServerLogger.h"

constexpr const uint32_t maxCachedConnections = 32;

std::string executeCommand(char* command)
{
    std::string result;
    FILE* fp = popen(command, "r");

    while (!feof(fp))
    {
        char ch;
        fread(&ch, sizeof(ch), 1, fp);
        result.append(&ch);
    }

    pclose(fp);

    return result;
}

int ReadMessage(Socket clientSocket, std::string& message)
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

int SendMessage(Socket clientSocket, std::string message)
{
    int size = message.size();
    Logger::log("Sending command with size = ", size, "\n");
    int writeResult = write(clientSocket, &size, sizeof(size));
    if (writeResult < size)
    {
        Logger::log("Attempt to write message resulted in a write() with value", writeResult, "\n");
        return -1;
    }

    Logger::log("Sending command: \"", message, "\"\n");
    writeResult = write(clientSocket, message.c_str(), size);
    if (writeResult < size)
    {
        Logger::log("Attempt to write message resulted in a write() with value", writeResult, "\n");
        return -1;
    }

    return 0;
}



void clientCommunication(Socket clientSocket)
{
    Logger::closeLogger();
    Logger::initLogger(std::string("client") + std::to_string(clientSocket) + ".logs");
    std::string exitMessage = "EXIT";
    int magic = 0xBEE;
    int receivedMagic;
    read(clientSocket, &receivedMagic, sizeof(receivedMagic));
    if (receivedMagic != magic)
    { // Invalid client; still send a exit message, just in case.
        SendMessage(clientSocket, exitMessage);
        return;
    }
    Logger::log("Magic confirmed. Started serving the client\n");

    while (true)
    {
        Logger::log("Waiting for message\n");
        int messageSize;
        std::string command;
        std::string cwd;
        if (ReadMessage(clientSocket, command))
        {
            break;
        }
        if (ReadMessage(clientSocket, cwd))
        {
            break;
        }

        Logger::log("Executing command \"", command, "\" with the working directory \"", cwd, "\".\n");

        if (command.compare(0, strlen("exit"), "exit") == 0)
        { // exit
            SendMessage(clientSocket, exitMessage);
            break;
        }

        //executeCommand(command);
        //free(command);
        
        if (SendMessage(clientSocket, command))
        {
            break;
        }
    }
    close(clientSocket);
    Logger::log("Client disconnected\n");
    Logger::closeLogger();
}


int main(int argc, const char* argv[])
{
    std::string ip;
    uint32_t port;
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        std::cout << "Defaulting to 127.0.0.1:1234" << std::endl;
        ip = "127.0.0.1";
        port = 1234;
    }
    else
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    Logger::initLogger("server-logs.txt");
    Logger::log("Starting server on ", ip, ":", port, "\n");

    Socket listenSocket;
    EVALUATE(listenSocket = socket(AF_INET, SOCK_STREAM, 0), -1, ==, 
            "Unable to create listen socket.");
    Logger::log("Created listening socket\n");

    sockaddr_in server = {}, clientInfo = {};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    EVALUATE(bind(listenSocket, (sockaddr*)&server, sizeof(sockaddr)),
            -1, ==, "Unable to bind listening socket to server");
    Logger::log("Bound listening socket to server\n");

    EVALUATE(listen(listenSocket, maxCachedConnections), -1, ==,
            "Unale to make listen socket listen");
    Logger::log("Listening socket ready to accept ", maxCachedConnections, " connections\n");

    while (true)
    {
        Socket clientSocket;
        uint32_t length = sizeof(clientInfo);

        Logger::log("Waiting for connections.\n");
        EVALUATE(clientSocket = accept(listenSocket, (sockaddr*)&clientInfo, &length),
                0, <, "Error while accepting client");
        
        // if (fork() == 0)
        // {
        //     close(listenSocket);
        //     clientCommunication(clientSocket);
        //     return;
        // }
        // else
        // {
        //     close(clientSocket);
        // }
        clientCommunication(clientSocket);
    }

    Logger::log("Closing listening socket\n");
    close(listenSocket);
    Logger::closeLogger();
}
