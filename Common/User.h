//
// Created by kkorzhanevskii on 22.05.22.
//
#pragma once
#include <string>
#include <fstream>
#include <utility>

/**
 * User data class
 *
 * This class stores name, login and password for one user.
 *
 * You can set/get any data using corresponding functions.
 */

class User {
public:
    User() = default;
    User(std::string  _login, std::string  _name, std::string  _password);

    const std::string& Login() const;
    void Login(const std::string& _login);

    const std::string& Name() const;
    void Name(const std::string& _name);

    const std::string& Password() const;
    void Password(const std::string& _password);

    friend std::istream& operator>> (std::istream& is, User& obj);
    friend std::ostream& operator<< (std::ostream& os, const User& obj);

private:
    std::string login;
    std::string name;
    std::string password;

};
