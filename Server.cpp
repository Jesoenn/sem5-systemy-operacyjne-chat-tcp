//
// Created by jakub on 2026-01-06.
//

#include "Server.h"
#include <iostream>
#include <thread>
#include <ws2tcpip.h>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

const int MAX_CLIENTS = 16;
const int MAX_MESSAGES = 50;

Server::Server(const std::string& ip, int port) :
        ip(ip), port(port) {}

void Server::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw std::invalid_argument("WSAStartup failed");
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        throw std::invalid_argument("Socket creation failed");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::invalid_argument("Bind failed");
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::invalid_argument("Listen failed");
    }

    std::cout << "Server: "<<ip<<":"<< port<<"\n";
    acceptClients();
}

bool Server::checkLimitExceeded(SOCKET client) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    if (clients.size() >= MAX_CLIENTS) {
        std::string msg = "FULL";
        send(client, msg.c_str(), msg.size(), 0);
        closesocket(client);
        return true;
    }
    return false;
}

bool Server::checkUsernameAvailable(SOCKET clientSocket, std::string username) {
    std::lock_guard<std::mutex> lock(usernamesMutex);
    for(std::string name: usernames){
        if(name == username){
            std::string msg = "Nickname taken";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            closesocket(clientSocket);
            return false;
        }
    }
    usernames.push_back(username);
    return true;
}

void Server::acceptClients() {
    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) continue;

        if(checkLimitExceeded(clientSocket))
            continue;

        //Read username
        char buffer[256];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            continue;
        }
        buffer[bytesReceived] = '\0';
        std::string username = buffer;

        if(!checkUsernameAvailable(clientSocket, username))
            continue;

        clientsMutex.lock();
        clients.push_back(clientSocket);
        clientsMutex.unlock();

        std::cout << username << " connected!\n";

        sendMessagesHistory(clientSocket);
        std::thread(&Server::receiveMessages, this, clientSocket).detach();
    }
}

std::string Server::getClientUsername(SOCKET client) {
    std::lock_guard<std::mutex> lock1(clientsMutex);
    std::lock_guard<std::mutex> lock2(usernamesMutex);

    for (int i=0; i<clients.size(); i++) {
        if (clients[i] == client){
            if (i < usernames.size())
                return usernames[i];
            else
                return "ERROR";
        }
    }
    return "ERROR";
}

void Server::receiveMessages(SOCKET client) {
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
    closesocket(client);
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    }

    {
        std::lock_guard<std::mutex> lock(usernamesMutex);
        usernames.erase(std::remove(usernames.begin(), usernames.end(), username), usernames.end());
    }
    std::cout << username << " disconnected\n";
}


void Server::sendBroadcast(const std::string& msg) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (SOCKET c : clients) {
        send(c, msg.c_str(), msg.size(), 0);
    }
}

void Server::sendMessagesHistory(SOCKET client) {
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



