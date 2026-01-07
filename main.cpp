// Serwer: so_projekt2.exe -s IP PORT
// Klient: so_projekt2.exe -c IP PORT NAZWA

#include "Server.h"
#include "Client.h"
#include <iostream>

void err_wrong_usage(){
    std::string err = "Usage:\n  Server: so_projekt2 -s IP PORT\n"
                      "  Client: so_projekt2 -c IP PORT NAME\n";
    throw std::invalid_argument(err);
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        err_wrong_usage();
    }

    std::string mode = argv[1];
    std::string ip = argv[2];
    int port = std::stoi(argv[3]);

    if (mode == "-s") {
        Server server(ip, port);
        server.start();
    } else if (mode == "-c") {
        if ( argc != 5 ){
            err_wrong_usage();
        }
        std::string name = argv[4];

        Client client(ip, port, name);
        client.start();
    } else {
        throw std::invalid_argument("Unknown mode");
    }

    return 0;
}
