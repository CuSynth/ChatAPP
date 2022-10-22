//
// Created by kkorzhanevskii on 22.05.22.
//

#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <QSqlDatabase>

#include "User.h"
#include "Message.h"

/** Database: class implementing connection to database.
 *
 */
class Database
{
public:
    Database(std::string  _host, std::string  _userName, std::string  _userPassword, std::string  _baseName);
    ~Database();

    void connect();

    void getUsers(std::vector<User>& users);
    void getRoomsMessages(std::vector<Message>& messages);
    void getUsersMessages(std::vector<Message>& messages);
    void getRooms(std::vector<User>& rooms);

    void addUser(User& user);
    void addMessage(Message& message);
    void addBan(BanMsg& ban);

    bool checkIfUserInRoom(const std::string& u, const std::string& r);
    bool checkIfUserInRoom(User& u, User& r);

    std::vector<std::string> getUserRooms(const std::string& u);
    std::vector<std::string> getUserRooms(const User& u) {
        return getUserRooms(u.Login());
    }


    bool isAdmin(const std::string& u, const std::string& r);
    bool isAdmin(User& u, User& r) {
        return isAdmin(u.Login(), r.Login());
    }

    bool isBanned(const std::string& u, const std::string& r);
    bool isBanned(User& u, User& r) {
        return isAdmin(u.Login(), r.Login());
    }

private:
    QSqlDatabase database;

    std::string host;
    std::string userName;
    std::string userPassword;
    std::string baseName;
};