#include "Common.h"
#include "ServerLogger.h"

constexpr const uint32_t maxCachedConnections = 32;
constexpr const uint32_t maxCommandSize = 255;

void clientCommunication(Socket clientSocket)
{
    Logger::closeLogger();
    Logger::initLogger(std::string("client") + std::to_string(clientSocket) + ".logs");
    char exitMessage[] = "EXIT";
    int magic = 0xFEEA;
    int receivedMagic;
    read(clientSocket, &receivedMagic, sizeof(receivedMagic));
    if (receivedMagic != magic)
    { // Invalid client; still send a exit message, just in case.
        write(clientSocket, exitMessage, sizeof(exitMessage));
        return;
    }
    
    char command[maxCommandSize];

    while (true)
    {
        read(clientSocket, command, sizeof(command));
        Logger::log("Received command: \"", command, "\"\n");

        if (strcmp(command, "exit") == 0)
        {
            write(clientSocket, exitMessage, sizeof(exitMessage));
            break;
        }

        write(clientSocket, command, sizeof(command));
    }

    close(clientSocket);
}


int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        return 0;
    }

    std::string ip = argv[1];
    uint32_t port = atoi(argv[2]);

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
        
        clientCommunication(clientSocket);

    }


    Logger::log("Closing listening socket\n");
    close(listenSocket);
    Logger::closeLogger();
}
