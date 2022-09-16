//
// Created by kkorzhanevskii on 22.05.22.
//

#pragma once
#include <string>
#include <fstream>
#include <utility>

class Message {
public:
    Message() = default;
    Message(std::string  _from, std::string  _to, std::string _text, std::string _time="", bool _grouped = false);

    const std::string& From() const;
    const std::string& To() const;
    const std::string& Text() const;
    const std::string& Time() const;
    void setGrouped(bool state) {
        grouped = state;
    }
    bool getGrouped() {
        return grouped;
    }

    friend std::istream& operator>> (std::istream& is, Message& obj) ;
    friend std::ostream& operator<< (std::ostream& os, const Message& obj);

private:
    std::string from;
    std::string to;
    std::string text;
    std::string time;

    bool grouped = false;
};

class BanMsg {
public:
    BanMsg() = default;
    BanMsg(std::string _who, std::string _where, std::string _whom, std::string _why) :
    who(_who), where(_where), whom(_whom), why(_why) {}

    const std::string& Who() const { return who; };
    const std::string& Where() const{ return where; };
    const std::string& Whom() const{ return whom; };
    const std::string& Why() const{ return why; };

    friend std::istream& operator>> (std::istream& is, BanMsg& obj) ;
    friend std::ostream& operator<< (std::ostream& os, const BanMsg& obj);


private:
    std::string who;
    std::string where;
    std::string whom;
    std::string why;
};


