//
// Created by jakub on 2026-01-06.
//

#include "Client.h"
#include <iostream>
#include <thread>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

Client::Client(const std::string& ip, int port, std::string name):
    ip(ip), port(port), name(name) {}

// Dodac polaczenie w innej funkcji. Jezeli uzytkownik istnieje to wyrzuca program

void Client::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        return;
    }

    std::cout << "Connected to server " << ip << ":" << port << "\n";

    std::thread(&Client::receiveMessages, this).detach();

    // Pętla wysyłania wiadomości do serwera
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        send(sock, msg.c_str(), msg.size(), 0);
    }
}

void Client::receiveMessages() {
    char buffer[512];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        std::cout << buffer << "\n";
    }
}

