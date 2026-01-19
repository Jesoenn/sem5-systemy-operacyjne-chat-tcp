//
// Created by jakub on 2026-01-06.
//

#include "Server.h"
#include <iostream>
#include <thread>
#include <algorithm>

const int MAX_CLIENTS = 16;
const int MAX_MESSAGES = 50;

Server::Server(const std::string& ip, int port) :
        ip(ip), port(port) {}

void Server::start() {
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        throw std::invalid_argument("Socket creation failed");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::invalid_argument("Bind failed");
    }

    if (listen(listenSocket, SOMAXCONN) == -1) {
        throw std::invalid_argument("Listen failed");
    }

    std::cout << "Server: "<<ip<<":"<< port<<"\n";
    acceptClients();
}

bool Server::checkLimitExceeded(int client) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    if (activeClients.size() >= MAX_CLIENTS) {
        std::string msg = "FULL";
        send(client, msg.c_str(), msg.size(), 0);
        close(client);
        return true;
    }
    return false;
}

bool Server::checkUsernameAvailable(int clientSocket, std::string username) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    std::string msg = "OK";

    for(const ActiveClient& activeClient: activeClients){
        if(username == activeClient.username){
            msg = "Nickname taken";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            close(clientSocket);
            return false;
        }
    }
    activeClients.push_back({clientSocket, username});
    send(clientSocket, msg.c_str(), msg.size(), 0);
    return true;
}

void Server::acceptClients() {
    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == -1) continue;

        if(checkLimitExceeded(clientSocket))
            continue;

        //Read username
        char buffer[256];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0) {
            close(clientSocket);
            continue;
        }
        buffer[bytesReceived] = '\0';
        std::string username = buffer;

        if(!checkUsernameAvailable(clientSocket, username))
            continue;

        std::cout << username << " connected!\n";

        sendMessagesHistory(clientSocket);
        std::thread(&Server::receiveMessages, this, clientSocket).detach();
    }
}

std::string Server::getClientUsername(int client) {
    std::lock_guard<std::mutex> lock1(clientsMutex);

    for (int i=0; i<activeClients.size(); i++) {
        if (activeClients[i].socket == client){
            return activeClients[i].username;
        }
    }

    return "ERROR";
}

void Server::receiveMessages(int client) {
    char buffer[512];
    std::string username= getClientUsername(client);

    while (true) {
        int bytesReceived = recv(client, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0)
            break;

        buffer[bytesReceived] = '\0';
        std::string msg = "["+getCurrentTime()+"] "+username+": "+buffer;
        std::cout << msg << "\n";

        saveMsg(msg);   // Save message to message history
        sendBroadcast(msg); // Send message to everybody
    }

    //Client disconnects
    close(client);
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto i = activeClients.begin(); i != activeClients.end(); i++) {
        if (i->socket == client) {
            activeClients.erase(i);
            break;
        }
    }

    std::cout << username << " disconnected\n";
}


void Server::sendBroadcast(const std::string& msg) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (ActiveClient c : activeClients) {
        send(c.socket, msg.c_str(), msg.size(), 0);
    }
}

void Server::sendMessagesHistory(int client) {
    std::lock_guard<std::mutex> lock(messagesMutex);
    for (std::string msg: messages) {
        msg += "\n";
        send(client, msg.c_str(), msg.size(), 0);
    }
}

std::string Server::getCurrentTime() {
    time_t timeStamp;
    time(&timeStamp);
    std::string currTime = ctime(&timeStamp);
    return currTime.substr(currTime.find(':')-2, 8);
}

void Server::saveMsg(const std::string& msg) {
    std::lock_guard<std::mutex> lock(messagesMutex);
    if (messages.size() >= MAX_MESSAGES)
        messages.erase(messages.begin());
    messages.push_back(msg);
}



