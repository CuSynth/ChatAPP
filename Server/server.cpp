//
// Created by kkorzhanevskii on 22.05.22.
//

#include <algorithm>
#include "server.h"

void Server::start(std::string dbHost, std::string dbUser, std::string dbPass, std::string dbBase) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(DEFAULT_PORT);
    Net net {addr};

    Database db{dbHost, dbUser, dbPass, dbBase};

    try {
        db.connect();

        db.getUsers(users);
        db.getUsersMessages(messages);
        db.getRoomsMessages(messages);
        db.getRooms(rooms);

        net.Bind();
        net.Listen();


        //server always works
        do {
            auto fds = net.waitForMultipleEvents();

            for(auto fd : fds) {
                Net::PacketType packetType = Net::PacketType::Unknown;
                net.Read<Net::PacketType>(packetType, fd);

                switch (packetType) {
                    case Net::PacketType::Login :
                        userLogin(net, fd);
                        break;
                    case Net::PacketType::Registration :
                        userRegistration(net, db, fd);
                        break;
                    case Net::PacketType::GetMessages :
                        sendMessages(net, db, fd);
                        break;
                    case Net::PacketType::AddMessage :
                        addMessage(net, db, fd);
                        break;
                    case Net::PacketType::getNameUsers :
                        sendAllUserNames(net, fd);
                        break;
                    case Net::PacketType::AddGroupMessage:
                        addGroupMessage(net, db, fd);
                        break;
                    case Net::PacketType::getNameRooms:
                        sendAllGroupNames(net, fd);
                        break;
                    case Net::PacketType::Ban:
                        addBan(net, db, fd);
                        break;
                    default:
                        break;
                }

                }
        } while (true);
    }
    catch(const std::exception& e) {
        throw std::runtime_error(e.what());
    }
}

void Server::userLogin(Net& net, int fdnum) {
    User u;
    net.Read<User>(u, fdnum);

    // Optimize
    for(auto &user : users) {
        if(user.Login() == u.Login() && user.Password() == u.Password()) {
            std::cout << "successfully" << std::endl;
            loginUsers[net.getFD(fdnum)] = std::make_unique<User>(user);
            net.Write<bool>(true, fdnum);
            net.Write<User>(user, fdnum);
            return;
        }
    }

    std::cout << "Login failed" << std::endl;
    net.Write<bool>(false, fdnum);
}

void Server::userRegistration(Net& net, Database& db, int fdnum) {
    User u;
    net.Read<User>(u, fdnum);

    // Optimize
    for(auto &user : users) {
        if(user.Login() == u.Login()) {
            std::cout << "Registration failed" << std::endl;
            net.Write<bool>(false, fdnum);
            return;
        }
    }

    users.push_back(u);
    db.addUser(u);

    std::cout << "Registration successfully done" << std::endl;

    loginUsers[net.getFD(fdnum)] = std::make_unique<User>(u);
    net.Write<bool>(true, fdnum);
}

void Server::sendMessages(Net& net, Database& db, int fdnum) {
    std::vector<Message> send_messages;
    auto groups = db.getUserRooms(loginUsers[net.getFD(fdnum)]->Login());

    for (auto& message : messages) {

        if ( ( message.From() == loginUsers[net.getFD(fdnum)]->Login() || message.To() == loginUsers[net.getFD(fdnum)]->Login()
            || (std::find(groups.begin(), groups.end(), message.To()) != groups.end()) )
            && !db.isBanned(loginUsers[net.getFD(fdnum)]->Login(), message.To())
        )
            send_messages.push_back(message);
    }

    net.Write<size_t>(send_messages.size(), fdnum);

    for (auto& message : send_messages) {
        net.Write<Message>(message, fdnum);
    }
}



void Server::addMessage(Net& net, Database& db, int fdnum) {
    Message message;
    net.Read<Message>(message, fdnum);

    messages.push_back(message);
    db.addMessage(message);
}

void Server::sendAllUserNames(Net& net, int fdnum) {
    std::vector<User> send_users;

    for (auto& user : users) {
        send_users.push_back({user.Login(),user.Name(),""});
    }

    net.Write<size_t>(send_users.size(), fdnum);

    for (auto& user : send_users) {
        net.Write<User>(user, fdnum);
    }
}

void Server::sendAllGroupNames(Net &net, int fdnum) {
    std::vector<User> send_;

    for (auto& room : rooms) {
        send_.push_back({room.Login(),room.Name(),""});
    }

    net.Write<size_t>(send_.size(), fdnum);

    for (auto& user : send_) {
        net.Write<User>(user, fdnum);
    }
}


void Server::addGroupMessage(Net &net, Database &db, int fdnum) {
    Message message;
    net.Read<Message>(message, fdnum);
    message.setGrouped(true);

    if(db.checkIfUserInRoom(message.From(), message.To())){
        messages.push_back(message);
        db.addMessage(message);
    }
}

void Server::addBan(Net& net, Database& db, int fdnum) {
    BanMsg message;
    net.Read<BanMsg>(message, fdnum);
    if(db.isAdmin(message.Whom(), message.Where()))
        db.addBan(message);
}