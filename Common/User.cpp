//
// Created by kkorzhanevskii on 22.05.22.
//

#include "User.h"

User::User(std::string _login, std::string _name, std::string _password) :
        login(std::move(_login)), name(std::move(_name)), password(std::move(_password)) {}

const std::string& User::Login() const {
    return login;
}

void User::Login(const std::string& _login) {
    login = _login;
}

const std::string& User::Name() const {
    return name;
}

void User::Name(const std::string& _name) {
    name = _name;
}

const std::string& User::Password() const {
    return  password;
}

void User::Password(const std::string &_password) {
    password = _password;
}

std::istream& operator>> (std::istream& is, User& obj) {
    std::getline(is, obj.login);
    std::getline(is, obj.name);
    std::getline(is, obj.password);
    return is;
}

std::ostream& operator<< (std::ostream& os, const User& obj) {
    os << obj.login << '\n';
    os << obj.name << '\n';
    os << obj.password;
    return os;
}
