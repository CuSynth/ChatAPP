//
// Created by kkorzhanevskii on 22.05.22.
//

#pragma once
#include <vector>
#include <memory>
#include <map>
#include "Net.h"
#include "Database.h"

/** TODO: Give more rules for administrators.
 *   Implement password encryption
 *
 */



/** Server: the server itself.
 *
 */
class Server {
public:
    /** start: connect server to database.
     *  Then, if connected, in loop wait for new connections/new data and process them
     *
     * @param dbHost [in] string with database host address
     * @param dbUser [in] string with database host username
     * @param dbPass [in] string with database host password
     * @param dbBase [in] string - the name of database to store data
     */
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
