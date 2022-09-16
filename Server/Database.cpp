//
// Created by kkorzhanevskii on 22.05.22.
//

#include <QSqlError>
#include <QSqlQuery>

#include "Database.h"

Database::Database(std::string _host, std::string _userName, std::string _userPassword, std::string _baseName) :
        host(std::move(_host)), userName(std::move(_userName)), userPassword(std::move(_userPassword)), baseName(std::move(_baseName)) {}

Database::~Database() {
    database.close();
}


void Database::connect() {
    database = QSqlDatabase::addDatabase("QPSQL");
    database.setHostName(QString::fromStdString(host));
    database.setUserName(QString::fromStdString(userName));
    database.setPassword(QString::fromStdString(userPassword));
    database.setDatabaseName(QString::fromStdString(baseName));

    bool state = database.open();
    if(!state)
        throw std::runtime_error(database.lastError().text().toStdString());

 std::cout << "Database successfully connected" << std::endl;
}

void Database::getUsers(std::vector<User>& users) {
    std::string query = "select * from users;";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));

    while (ret.next()) {
        if (!ret.isValid())
            throw std::runtime_error(ret.lastError().text().toStdString());

        users.push_back({ret.value("login").toString().toStdString(),
                         ret.value("name").toString().toStdString(),
                         ret.value("password").toString().toStdString()});
    }

//    std::cout<<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}

void Database::getRooms(std::vector<User>& rooms) {
    std::string query = "select * from rooms;";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));

    while (ret.next()) {
        if (!ret.isValid())
            throw std::runtime_error(ret.lastError().text().toStdString());

        rooms.push_back({ret.value("link").toString().toStdString(),
                         ret.value("name").toString().toStdString(),
                         ""});
    }

//    std::cout<<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}

void Database::getUsersMessages(std::vector<Message>& messages) {
    std::string query = "select _from, user_to, text, time::timestamp from messages where user_to is not null";
//    std::cout <<":Q "<<query;
    auto ret = database.exec(QString::fromStdString(query));

    while (ret.next()) {
        if (!ret.isValid())
            throw std::runtime_error(ret.lastError().text().toStdString());

        messages.push_back({ret.value("_from").toString().toStdString(),
                         ret.value("user_to").toString().toStdString(),
                         ret.value("text").toString().toStdString(),
                         ret.value("time").toString().toStdString()});
    }
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}

void Database::getRoomsMessages(std::vector<Message>& messages)
{
    std::string query = "select _from, room_to, text, time::timestamp from messages where room_to is not null";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));

    while (ret.next()) {
        if (!ret.isValid())
            throw std::runtime_error(ret.lastError().text().toStdString());

        messages.push_back({ret.value("_from").toString().toStdString(),
                            ret.value("room_to").toString().toStdString(),
                            ret.value("text").toString().toStdString(),
                            ret.value("time").toString().toStdString(),
                            true});
    }
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}



void Database::addUser(User& user) {
    std::string query;
    query = "insert into "
            "users(login, name, password) values('"
            + user.Login() + "', '"
            + user.Name() + "', '"
            + user.Password() + "')";
//    std::cout <<"Q "<<query;
    auto ret= database.exec(QString::fromStdString(query));
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";

}



void Database::addMessage(Message& message) {
    std::string query;
    if(message.getGrouped()) {
        query = "insert into messages(_from, room_to, text) "
                "values('"
                + message.From() + "', '"
                + message.To() + "', '"
                + message.Text() + "')";
    }
    else {
        query = "insert into messages(_from, user_to, text) "
                "values('"
                + message.From() + "', '"
                + message.To() + "', '"
                + message.Text() + "')";
    }

//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}

bool Database::checkIfUserInRoom(User &u, User &r) {
    return checkIfUserInRoom(u.Login(), r.Login());
}

bool Database::checkIfUserInRoom(const std::string &u, const std::string &r) {
    std::string query;
    query = "select exists(select * from users_in_rooms where user_login = '"
            + u + "' and room_link = '"
            + r + "');";
//    std::cout <<"Q "<<query;
    auto ret = database.exec(QString::fromStdString(query));
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
    ret.next();
    return ret.value("exists").toBool();
}

std::vector<std::string> Database::getUserRooms(const std::string &u) {
    std::vector<std::string> to_ret;
    std::string query;
    query = "select room_link "
            "from users_in_rooms "
            "where user_login = '"
            + u + "';";

//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";

    while (ret.next()) {
        if (!ret.isValid())
            throw std::runtime_error(ret.lastError().text().toStdString());

        to_ret.push_back(ret.value("room_link").toString().toStdString());
    }
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";

    return to_ret;
}

bool Database::isAdmin(const std::string& u, const std::string& r) {
    std::string query;
    query = "select exists(select * from admins where _login = '"
            + u + "' and _where = '"
            + r + "');";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));

//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
    ret.next();
    return ret.value("exists").toBool();
}

bool Database::isBanned(const std::string& u, const std::string& r) {
    std::string query;
    query = "select exists(select * from bans where who = '"
            + u + "' and _where = '"
            + r + "');";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));

//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
    ret.next();
    return ret.value("exists").toBool();
}

void Database::addBan(BanMsg& ban) {
    std::string query;
    query = "insert into bans (who, _where, whom, reason) "
            "values('"
            + ban.Who() + "', '"
            + ban.Where() + "', '"
            + ban.Whom() + "', '"
            + ban.Why() + "')";
//    std::cout <<"Q: "<<query;
    auto ret = database.exec(QString::fromStdString(query));
//    std::cout <<"Err: "<< ret.lastError().text().toStdString() <<"\n";
}
