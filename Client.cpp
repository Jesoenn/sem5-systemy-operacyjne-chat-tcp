//
// Created by jakub on 2026-01-06.
//

#include "Client.h"
#include <iostream>
#include <thread>
#include <utility>
#include <ws2tcpip.h>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <mutex>

using namespace ftxui;

#pragma comment(lib, "Ws2_32.lib")

Client::Client(std::string  ip, int port, std::string name):
    ip(std::move(ip)), port(port), name(std::move(name)) {}



void Client::startUI() {
    using namespace ftxui;

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    std::vector<std::string> messages;
    std::mutex messagesMutex;

    std::string inputBuffer;
    auto input = Input(&inputBuffer, "Type message...");

    // Dodaj CatchEvent do inputa
    input = CatchEvent(input, [&](Event event) {
        if (event == Event::Return && !inputBuffer.empty()) {
            // Wyślij do serwera
            send(sock, inputBuffer.c_str(), inputBuffer.size(), 0);

            // Nie dodajemy "Me: ..." lokalnie, bo serwer zwróci to, co pokazać
            inputBuffer.clear();
            return true;
        }
        return false;
    });


    // Górne okno - wyświetlanie wiadomości
    auto messagesBox = Renderer([&] {
        Elements elements;
        std::lock_guard<std::mutex> lock(messagesMutex);

        // pokaż tylko ostatnie 50 wiadomości
        if (messages.size() > 50) {
            messages.erase(messages.begin(), messages.begin() + (messages.size() - 50));
        }

        for (auto& msg : messages)
            elements.push_back(text(msg));

        return vbox(elements) | border | flex;
    });

    // Dolne okno - input
    auto inputBox = Renderer(input, [&] {
        return hbox({text("> ") | bold, input->Render() | flex}) | border;
    });

    // Kontener wszystkich komponentów
    auto layout = Container::Vertical({messagesBox, inputBox});

    std::thread receiver([&] {
        char buffer[512];
        while (!connectionEnded) {
            int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) break;

            buffer[bytesReceived] = '\0';
            std::string msg(buffer);

            {
                std::lock_guard<std::mutex> lock(messagesMutex);
                messages.push_back(msg);
            }

            // Wymuszenie natychmiastowego odświeżenia GUI
            screen.PostEvent(Event::Custom);
        }
        connectionEnded = true;
    });


    screen.Loop(layout);
    receiver.join();
}





void Client::start() {
    setUpConnection();
    sendUsername();
    startUI();

//    std::thread receiverThread(&Client::receiveMessages, this);
//    sendMessages();
//    receiverThread.join();
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

