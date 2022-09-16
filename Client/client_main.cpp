#include "Client.h"

int main(int argc, char *argv[]) {

    try {
        Client client;
        client.start();
    }
    catch(std::exception& e) {
        std::cerr << "\033[31m";
        std::cerr << e.what() << std::endl;
        std::cerr << "\033[0m";
    }

    return 0;
}
