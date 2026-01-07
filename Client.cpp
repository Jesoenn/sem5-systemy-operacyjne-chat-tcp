//
// Created by jakub on 2026-01-06.
//

#include "Client.h"
#include <iostream>
#include <thread>
#include <utility>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

Client::Client(std::string  ip, int port, std::string name):
    ip(std::move(ip)), port(port), name(std::move(name)) {}

void Client::start() {
    setUpConnection();
    sendUsername();

    std::thread receiverThread(&Client::receiveMessages, this);
    sendMessages();
    receiverThread.join();
}

void Client::receiveMessages() {
    char buffer[512];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        std::cout <<buffer << "\n";
    }
    std::cout<<"Connection ended by server\n";
    connectionEnded = true;
}

void Client::setUpConnection() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw std::invalid_argument("WSAStartup failed\n");
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        throw std::invalid_argument("Socket creation failed\n");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::invalid_argument("Connection failed\n");
    }
    std::cout << "Connected to server " << ip << ":" << port << "\n";
}


void Client::sendUsername() {
    send(sock, name.c_str(), name.size(), 0);

    char buffer[512];
    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0); // zostawiamy miejsce na '\0'
    if (bytesReceived <= 0) {
        closesocket(sock);
        throw std::invalid_argument("Connection ended by server");
    }

    buffer[bytesReceived] = '\0';
    std::string response(buffer);

    if (response == "FULL") {
        closesocket(sock);
        throw std::invalid_argument("Server is full");
    } else if (response == "Nickname taken") {
        closesocket(sock);
        throw std::invalid_argument("Nickname is already taken");
    }
}


void Client::sendMessages() {
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if(connectionEnded)
            break;
        send(sock, msg.c_str(), msg.size(), 0);
    }
}

