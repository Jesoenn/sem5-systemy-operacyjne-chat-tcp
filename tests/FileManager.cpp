//
// Created by jakub on 2026-01-19.
//

#include "FileManager.h"
#include <fstream>
#include <iostream>

void FileManager::addMessageReceived(const std::string& username, int msgSize, int time) {
    std::ofstream file("messageTimerTest.txt", std::ios::app);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open messageTimerTest.txt");
    }

    file << username << '\t' << msgSize << '\t' << time << '\n';
    file.close();
}
