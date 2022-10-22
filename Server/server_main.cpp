#include <iostream>
#include "server.h"
#include "Database.h"


int main(int argc, char *argv[]) {

    try {
        if(argc != 5) {
            throw std::invalid_argument("Wrong param count");
        }

        Server server;
        server.start("localhost", "postgres", "pass", "chatting");

    }
    catch(const std::exception& e) {
        std::cerr << "\033[31m";
        std::cerr << e.what() << std::endl;
        std::cerr << "\033[0m";
        return -1;
    }

    return 0;
}
