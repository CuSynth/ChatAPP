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


/** TODO: implement server/client difference using templates
 *   Implement tcp/ip using Qt/Boost libs
 *   Implement mechanism of turning on/off debug mode
 *   Use multithreading for accepting data from different clients.
 */


/** Net: class implementing tcp/ip connection for server and client.
 *
 */

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

    /** Constructor
     *
     * @param [in] _addr is the address of new connection
     * @param [in] is_server bool value. If true - the object will be a server part of connection
     */
    Net(sockaddr_in& _addr, bool is_server=true);
    ~Net();

    // Wrappers for server

    /** Bind: bind socket_fd
     *
     */
    void Bind();

    /** Listen: start to listen for new connections or new data from some connection
     *
     */
    void Listen();

    /** waitForMultipleEvents: start waiting for any event.
     * If there is a new incoming connection - accept it and put its fd into 'fds'
     *
     * @return Returns the vector of integers: numbers of connections with some new data on them
     */
    std::vector<int> waitForMultipleEvents();

    // Wrapper for client

    /** Connect: connect to socket_fd
     *
     */
    void Connect();


    /** Write: write some data to/from the server
     *
     * @tparam T type of writing data
     * @param t [in] data to write
     * @param fdnum [in] the number of client to write from the server
     */
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

    /** Read: write some data from the server or client
       *
       * @tparam T type of reading data
       * @param t [out] variable to write
       * @param fdnum [in] the number of client to read from in the server
       */
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

    /** closeConnection: close given connection
     *
     * @param fdnum [in] the number of connection to close
     */
    void closeConnection(int fdnum);

    int getFD(int fdnum) {
        return fds[fdnum].fd;
    }

private:
    sockaddr_in addr;
    socklen_t addr_length;
    int socket_fd;  // Server socket fd
    int connection; // Fd to rw in client

    struct pollfd fds[200]; // Array of structs with an information about sockets for clients
    int num_fd; // The number of descriptors in 'fds'

};
