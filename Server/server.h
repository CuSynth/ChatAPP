//
// Created by kkorzhanevskii on 22.05.22.
//

#pragma once
#include <vector>
#include <memory>
#include <map>
#include "Net.h"
#include "Database.h"

class Server
{
public:
    void start(std::string dbHost, std::string dbUser, std::string dbPass, std::string dbBase);

private:
    void userLogin(Net& net, int fdnum);
    void userRegistration(Net& net, Database& db, int fdnum);

    void sendMessages(Net& net, Database& db, int fdnum);
    void sendAllUserNames(Net& net, int fdnum);
    void sendAllGroupNames(Net& net, int fdnum);

    void addMessage(Net& net, Database& db, int fdnum);
    void addGroupMessage(Net& net, Database& db, int fdnum);
    void addBan(Net& net, Database& db, int fdnum);

    std::vector<User> users;
    std::vector<User> rooms;
    std::vector<Message> messages;
    std::map<int, std::unique_ptr<User>> loginUsers;
};
