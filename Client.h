//
// Created by jakub on 2026-01-06.
//

#ifndef SO_PROJEKT2_CLIENT_H
#define SO_PROJEKT2_CLIENT_H


#include <string>
#include <winsock2.h>
#include <atomic>
#include <vector>
#include <ftxui/component/screen_interactive.hpp>
using namespace ftxui;

class Client {
private:
    std::vector<std::string> messages;
    std::mutex messagesMutex;
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    std::atomic<bool> connectionEnded = false;
    SOCKET sock;
    std::string ip, name;
    int port;

    void receiveMessages();
    void setUpConnection();
    void sendUsername();
    void sendMessages(const std::string& msg);
    void startUI();

public:
    Client(std::string  ip, int port, std::string name);
    void start();
};

#endif //SO_PROJEKT2_CLIENT_H
