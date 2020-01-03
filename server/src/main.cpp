#include "Common.h"
#include "ServerLogger.h"

constexpr const uint32_t maxCachedConnections = 32;

std::string executeCommand(const std::string& command, const std::string& cwd)
{
    std::string result;

    int p[2];
    EVALUATE(pipe(p), 0, !=, "Unable to create pipes");
    int pid;
    EVALUATE(pid = fork(), 0, <, "Unable to fork");

    if (pid == 0)
    { // Child
        close(p[0]);
        chdir(cwd.c_str()); // Changes the current working directory
        dup2(p[1], STDOUT);
        execl("/bin/bash", "bash", "-c", command.c_str(), NULL);
        exit(1);
    }

    // Parent
    int status;
    EVALUATE(wait(&status), -1, ==, "Wait failed");
    close(p[1]);

    if (status != 0)
    {
        return "Unable to run command \"" + command + "\"\n";
    }


    char ch;
    int readResult = read(p[0], &ch, sizeof(ch));
    while (readResult)
    {
        result.append(&ch);
        readResult = read(p[0], &ch, sizeof(ch));
    }

    close(p[0]);

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



void clientCommunication(Socket clientSocket, int numClient)
{
    Logger::closeLogger();
    Logger::initLogger(std::string("client") + std::to_string(numClient) + ".logs");
    std::string exitMessage = "EXIT";
    int magic = 0xBEE;
    int receivedMagic;
    read(clientSocket, &receivedMagic, sizeof(receivedMagic));
    if (receivedMagic != magic)
    { // Invalid client; still send a exit message, just in case.
        SendMessage(clientSocket, exitMessage);
        return;
    }
    char cwdPath[MAX_ASCII] = "/";
    std::string cwd;
    Logger::log("Magic confirmed. Started serving the client\n");
    getcwd(cwdPath, MAX_ASCII);
    Logger::log("Sending CWD = \"", cwd, "\"");
    cwd = std::string(cwdPath);
    SendMessage(clientSocket, cwd);

    while (true)
    {
        Logger::log("Waiting for message\n");
        int messageSize;
        std::string command;
        cwd = std::string(); // clear the cwd
        try
        {
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

            std::string result = executeCommand(command, cwd);
            Logger::log("Command result = \"", result, "\"\n");

            if (SendMessage(clientSocket, result))
            {
                break;
            }
        }
        catch(const std::exception& e)
        {
            Logger::log(e.what());
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

    int numClient = 0;
    while (true)
    {
        Socket clientSocket;
        uint32_t length = sizeof(clientInfo);

        Logger::log("Waiting for connections.\n");
        EVALUATE(clientSocket = accept(listenSocket, (sockaddr*)&clientInfo, &length),
                0, <, "Error while accepting client");
        numClient++;
        if (fork() == 0)
        {
            close(listenSocket);
            clientCommunication(clientSocket, numClient);
            return 0;
        }
        else
        {
            close(clientSocket);
        }
    }

    Logger::log("Closing listening socket\n");
    close(listenSocket);
    Logger::closeLogger();
}
