//
// Created by kkorzhanevskii on 22.05.22.
//

#include "Net.h"

Net::Net(sockaddr_in& _addr, bool is_server) : addr(_addr) {

    addr_length = sizeof(addr);
    // Open socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        std::cerr << "sock -1 ret\n";
        throw std::runtime_error(std::strerror(errno));
    }

    if(is_server) {
        int on = 1;
        // Set socket as nonblocking
        if (ioctl(socket_fd, FIONBIO, (char *)&on)  == -1) {
            std::cerr << "sock ioctl -1 ret\n";
            throw std::runtime_error(std::strerror(errno));
        }
    }

    num_fd = 1;
    memset(fds, 0 , sizeof(fds));

    // Set the first fd as current connection
    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;

    // Debug using std::cout, why not :)
    std::cout << "Socket " << socket_fd << " opened" << std::endl;
}

Net::~Net() {
    std::cout << "Socket " << socket_fd << " closed" << std::endl;
    close(socket_fd);
}

void Net::Bind() {
    int result = bind(socket_fd, reinterpret_cast<struct sockaddr*>(&addr), addr_length);
    if (result == -1) {
        std::cerr << "bind -1 ret\n";
        throw std::runtime_error(std::strerror(errno));
    }
}

void Net::Listen() {
    int result = listen(socket_fd, SOMAXCONN);
    if (result == -1) {
        std::cerr << "listen -1 ret\n";
        throw std::runtime_error(std::strerror(errno));
    }
}

void Net::Connect() {
    int result = connect(socket_fd, reinterpret_cast<struct sockaddr*>(&addr), addr_length);
    if (result == -1) {
        std::cerr << "in client connect -1 ret\n";
        throw std::runtime_error(std::strerror(errno));
    }

    connection = socket_fd;
    std::cout << "Connection " << connection << " opened" << std::endl;
}

void Net::closeConnection(int fdnum) {
    std::cout << "Connection " << (fdnum < 0 ? connection : fds[fdnum].fd) << " closed" << std::endl;
    close(fds[fdnum].fd);

    if(fdnum != 0 && fdnum != -1) {
        size_t shift = 0;
        for(size_t i = 0; i < num_fd; ++i) {
            if(i==fdnum) {
                shift = 1;
                --num_fd;
                fds[num_fd].revents = 0;
            }

            fds[i].fd = fds[i+shift].fd;
        }
    }
}

std::vector<int> Net::waitForMultipleEvents() {

    std::vector<int> ret;

    int changed = poll(fds, num_fd, -1);
    if (changed == -1) {
        std::cerr << "poll -1 ret\n";
        throw std::runtime_error(std::strerror(errno));
    }

    for (size_t i = 0; i < num_fd; ++i) {
        if(fds[i].revents == 0){
            continue;
        }

        if(fds[i].fd == socket_fd) {

            do {
                int new_fd = accept(socket_fd, NULL, NULL);
                if(new_fd == -1) {
                    if (errno != EWOULDBLOCK) {	// EWOULDBLOCK == no sockets to accept
                        std::cerr << "accept -1 ret\n";
                        throw std::runtime_error(std::strerror(errno));
                    }

                    break;
                }
                std::cout << "New incoming connection fd " << new_fd << std::endl;

                fds[num_fd].fd = new_fd;
                fds[num_fd].events = POLLIN;
                ++num_fd;

                int on = 1;
                if (ioctl(new_fd, FIONBIO, (char *)&on)  == -1) {
                    std::cerr << "ioctl -1 ret\n";
                    throw std::runtime_error(std::strerror(errno));
                }
            } while (true);
        }
        else { // There is a socket to read
            std::cout << "There is an information into fd " << fds[i].fd <<std::endl;
            ret.push_back(i);
        }
    }

    return ret;
}
