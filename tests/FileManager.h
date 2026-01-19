//
// Created by jakub on 2026-01-19.
//

#ifndef SO_PROJEKT2_FILEMANAGER_H
#define SO_PROJEKT2_FILEMANAGER_H

#include <string>

class FileManager {
public:
    void addMessageReceived(const std::string& username, int msgSize, int time);
};


#endif //SO_PROJEKT2_FILEMANAGER_H
