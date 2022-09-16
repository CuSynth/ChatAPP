//
// Created by kkorzhanevskii on 22.05.22.
//

#include "Message.h"

Message::Message(std::string _from, std::string _to, std::string _text, std::string _time, bool _grouped) :
        from(std::move(_from)), to(std::move(_to)),
        text(std::move(_text)), time(_time), grouped(_grouped) {}

const std::string& Message::From() const {
    return from;
}

const std::string& Message::To() const {
    return to;
}

const std::string& Message::Text() const {
    return text;
}

const std::string &Message::Time() const {
    return time;
}

std::istream& operator>> (std::istream& is, Message& obj) {
    std::getline(is, obj.from);
    std::getline(is, obj.to);
    std::getline(is, obj.text);
    std::getline(is, obj.time);

    if(obj.to[0] == '@')
        obj.setGrouped(true);

    return is;
}

std::ostream& operator<< (std::ostream& os, const Message& obj) {
    os << obj.from << '\n';
    os << obj.to << '\n';
    os << obj.text << '\n';
    os << obj.time;

    return os;
}




std::istream& operator>> (std::istream& is, BanMsg& obj) {
    std::getline(is, obj.who);
    std::getline(is, obj.where);
    std::getline(is, obj.whom);
    std::getline(is, obj.why);
    return is;
}

std::ostream& operator<< (std::ostream& os, const BanMsg& obj) {
    os << obj.who << '\n';
    os << obj.where << '\n';
    os << obj.whom << '\n';
    os << obj.why;
    return os;
}
