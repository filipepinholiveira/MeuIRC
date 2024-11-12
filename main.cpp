#include "server.hpp"
#include <iostream>
#include <cstdlib>  // Para std::stoi no C++98, usamos atoi

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <porta> <senha>\n";
        return 1;
    }

    // Converter a porta de string para inteiro com atoi
    int port = atoi(argv[1]);
    std::string password = argv[2];

    Server ircServer(port, password);
    ircServer.start();

    return 0;
}
