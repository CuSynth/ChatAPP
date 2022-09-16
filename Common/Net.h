//
// Created by kkorzhanevskii on 22.05.22.
//

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include "User.h"
#include "Message.h"

constexpr size_t DEFAULT_PORT = 34567;

class Net {
public:
    enum PacketType {
        Login = 1,
        Registration,
        AddMessage,
        AddGroupMessage,
        GetMessages,
        getNameUsers,
        getNameRooms,
        Ban,
        Exit,
        Unknown
    };


    Net(sockaddr_in& _addr, bool is_server=true);
    ~Net();

    // Wrappers for server
    void Bind();
    void Listen();
    std::vector<int> waitForMultipleEvents();

    // Wrapper for client
    void Connect();


    template<typename T>
    std::enable_if_t<std::is_same_v<T, bool>
                  || std::is_same_v<T, size_t>
                  || std::is_same_v<T, PacketType>, void>
    Write(const T& t, int fdnum) {
        ssize_t bytes = write((fdnum < 0 ? connection : fds[fdnum].fd), reinterpret_cast<const char*>(&t), sizeof(t));
        if (bytes == -1)
            throw std::runtime_error(std::strerror(errno));
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, User>
                     || std::is_same_v<T, Message>
                     || std::is_same_v<T, BanMsg>, void>
    Write(const T& t, int fdnum) {
        std::stringstream ss;
        ss << t;
        size_t data_size = ss.str().size();

        //write size packet with class
        Write<size_t>(data_size, fdnum);

        ssize_t bytes = write((fdnum < 0 ? connection : fds[fdnum].fd), ss.str().c_str(), data_size);
        if (bytes == -1)
            throw std::runtime_error(std::strerror(errno));
    }


    template<typename T>
    std::enable_if_t<std::is_same_v<T, User>
                     || std::is_same_v<T, Message>
                     || std::is_same_v<T, BanMsg>, void>
    Read(T& t, int fdnum) {

        std::stringstream ss;
        size_t data_size;

        //read size packet with class
        Read<size_t>(data_size, fdnum);

        char *buffer = new char[data_size];
        ssize_t bytes = read((fdnum < 0 ? connection : fds[fdnum].fd), buffer, data_size);

        if (bytes == -1) {
            delete[] buffer;
            throw std::runtime_error(std::strerror(errno));
        }

        ss.write(buffer, data_size);
        ss >> t;

        delete[] buffer;

        if(bytes == 0)
            closeConnection(fdnum);

    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, bool>
                     || std::is_same_v<T, size_t>
                     || std::is_same_v<T, PacketType>, void>
    Read(T& t, int fdnum) {
        ssize_t bytes = read((fdnum < 0 ? connection : fds[fdnum].fd), reinterpret_cast<char*>(&t), sizeof(t));
        if (bytes == -1)
            throw std::runtime_error(std::strerror(errno));

        if(bytes == 0)
            closeConnection(fdnum);
    }

    void closeConnection(int fdnum);

    int getFD(int fdnum) {
        return fds[fdnum].fd;
    }

private:
    sockaddr_in addr;
    socklen_t addr_length;
    int socket_fd;  // Server socket fd
    int connection; // Fd to rw in client

    struct pollfd fds[200];
    int num_fd;

};
//
//template <typename T>
//void Net::Read(T& t, int fdnum) {
//
//    std::stringstream ss;
//    size_t data_size;
//
//    //read size packet with class
//    simpleRead<size_t>(data_size, fdnum);
//
//    char *buffer = new char[data_size];
//    ssize_t bytes = read((fdnum < 0 ? connection : fds[fdnum].fd), buffer, data_size);
//
//    if (bytes == -1) {
//        delete[] buffer;
//        throw std::runtime_error(std::strerror(errno));
//    }
//
//    ss.write(buffer, data_size);
//    ss >> t;
//
//    delete[] buffer;
//
//    if(bytes == 0)
//        closeConnection(fdnum);
//
//}
//
//template <typename T> void Net::simpleRead(T& t, int fdnum) {
//    ssize_t bytes = read((fdnum < 0 ? connection : fds[fdnum].fd), reinterpret_cast<char*>(&t), sizeof(t));
//    if (bytes == -1)
//        throw std::runtime_error(std::strerror(errno));
//
//    if(bytes == 0)
//        closeConnection(fdnum);
//}
