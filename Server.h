//
// Created by jakub on 2026-01-06.
//

#ifndef SO_PROJEKT2_SERVER_H
#define SO_PROJEKT2_SERVER_H
#include <string>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

struct ActiveClient {
    int socket;
    std::string username;
};

class Server {
private:
    std::mutex clientsMutex;
    std::mutex messagesMutex;

    std::vector<ActiveClient> activeClients;
    int listenSocket;
    std::vector<std::string> messages; // Messages last 50: format: [HH:MM:SS] username: message
    std::string ip;
    int port;

    void acceptClients(); // Accepting clients in loop -> limit 16!
    bool checkUsernameAvailable(int clientSocket, std::string username); // Check if username is already connected
    void receiveMessages(int client); // Receive messages from clients
    void sendMessagesHistory(int client); // Send all messages to newly connected client
    void sendBroadcast(const std::string& msg); // Send new message to all clients

    void saveMsg(const std::string& msg); //Saves message to messages

    bool checkLimitExceeded(int client); // Check number of clients connected
    std::string getCurrentTime();
    std::string getClientUsername(int client);


public:
    Server(const std::string& ip, int port);
    void start();
};


#endif //SO_PROJEKT2_SERVER_H
