//
// Created by kkorzhanevskii on 23.05.22.
//

#include "Client.h"

void Client::start(std::string host) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host.c_str());
    addr.sin_port = htons(DEFAULT_PORT);

    Net net = Net(addr, false);

    try {
        net.Connect();
        showStartMenu(net);
    }
    catch(const std::exception& e) {
        std::cerr << "\033[31m";
        std::cerr << e.what() << std::endl;
        std::cerr << "\033[0m";
    }
}

void Client::showStartMenu(Net& net) {
    char choice;
    do {
        std::cout << "\033[1;34m";
        std::cout << "----------------" << std::endl;
        std::cout << "(1) Login" << std::endl;
        std::cout << "(2) Registration" << std::endl;
        std::cout << "(q) Exit" << std::endl;
        std::cout << "\033[0;36m";
        std::cout << ">> ";
        std::cin >> choice;
        std::cout << "\033[0m";

        switch (choice) {
            case '1':
                showLoginMenu(net);
                break;
            case '2':
                showRegistrationMenu(net);
                break;
            default:
                break;
        }
    } while (choice != 'q');
}

void Client::showLoginMenu(Net& net) {
    std::string login, pass;
    std::cout << "\033[34m"<< "----------------" << std::endl;
    std::cout << "Login    : " << "\033[36m";
    std::cin.ignore();
    getline(std::cin, login);
    std::cout << "\033[34m" << "Password : " << "\033[36m";
    getline(std::cin, pass);
    std::cout << "\033[0m";

    user_.Login(login);
    user_.Password(pass);

    net.Write<Net::PacketType>(Net::PacketType::Login, -1);
    net.Write<User>(user_, -1);

    bool isLoginSuccessfully = false;
    net.Read<bool>(isLoginSuccessfully, -1);

    if(isLoginSuccessfully) {
        net.Read<User>(user_, -1);
        showChatMenu(net);
    }
    else {
        std::cout << "\033[31m" << "Login failed" << "\033[0m" << std::endl;
    }
}



void Client::showRegistrationMenu(Net& net) {
    std::string login,  name, pass;
    std::cout << "\033[34m"<< "----------------" << std::endl;
    std::cout << "Login    : " << "\033[36m";
    std::cin.ignore();
    getline(std::cin, login);
    std::cout << "\033[34m" << "Name     : " << "\033[36m";
    getline(std::cin, name);
    std::cout << "\033[34m" << "Password : " << "\033[36m";
    getline(std::cin, pass);
    std::cout << "\033[0m";

    user_.Login(login);
    user_.Name(name);
    user_.Password(pass);

    net.Write<Net::PacketType>(Net::PacketType::Registration, -1);
    net.Write<User>(user_, -1);

    bool isRegistrationSuccessfully = false;
    net.Read<bool>(isRegistrationSuccessfully, -1);

    if(isRegistrationSuccessfully) {
        showChatMenu(net);
    }
    else {
        std::cout << "\033[31m" << "Registration failed" << "\033[0m" << std::endl;
    }
}



void Client::showChatMenu(Net& net) {
    std::cout << "\033[32m" << "Hi, " << user_.Name() << "!" << "\033[0m" << std::endl;

    char choice;
    do {
        std::cout << "\033[32m" << "Menu: (1)Show chat | (2)Send message to user | (3)Send message to group \n"
                                   "| (4)Users | (5)Rooms | (6)Ban | (q)Logout" << std::endl;
        std::cout << "\033[36m";
        std::cout << ">> ";
        std::cin >> choice;
        std::cout << "\033[0m";

        switch (choice)
        {
            case '1':
                showChat(net);
                break;
            case '2':
                sendMessageToUsr(net);
                break;
            case '3':
                sendMessageToGroup(net);
                break;
            case '4':
                showAllUsers(net);
                break;
            case '5':
                showAllRooms(net);
                break;
            case '6':
                banUsr(net);
                break;
            default:
                break;
        }
    } while (choice != 'q');
}

void Client::banUsr(Net &net) {
    std::string who, where, why;

    std::cout << "\033[33m" << "Who: ";
    std::cin.ignore();
    getline(std::cin, who);
    std::cout << "Where: ";
    getline(std::cin, where);
    std::cout << "Why: ";
    getline(std::cin, why);
    std::cout << "\033[0m";

    BanMsg msg{who, where, user_.Login(), why};

    net.Write(Net::PacketType::Ban, -1);
    net.Write(msg, -1);
}

void Client::sendMessageToUsr(Net& net) {
    std::string to, text;

    std::cout << "\033[33m" << "Login to: ";
    std::cin.ignore();
    getline(std::cin, to);
    std::cout << "Text: ";
    getline(std::cin, text);
    std::cout << "\033[0m";

    Message message = {user_.Login(), to, text};

    net.Write<Net::PacketType>(Net::PacketType::AddMessage, -1);
    net.Write<Message>(message, -1);
}

void Client::sendMessageToGroup(Net &net) {
    std::string to, text;

    std::cout << "\033[33m" << "Group link: ";
    std::cin.ignore();
    getline(std::cin, to);
    std::cout << "Text: ";
    getline(std::cin, text);
    std::cout << "\033[0m";

    Message message = {user_.Login(), to, text, "", true};

    net.Write<Net::PacketType>(Net::PacketType::AddGroupMessage, -1);
    net.Write<Message>(message, -1);
}

void Client::showAllUsers(Net& net) {
    net.Write<Net::PacketType>(Net::PacketType::getNameUsers, -1);

    size_t count;
    net.Read<size_t>(count, -1);
    for (size_t i = 0; i < count; ++i) {
        std::cout << "\033[33m" << "------------------------------" << std::endl;
        User user;
        net.Read<User>(user, -1);

        std::cout << "Name: " << user.Name() << std::endl;
        std::cout << "Login: " << user.Login() << std::endl;
    }
    std::cout << "\033[0m";
}

void Client::showChat(Net& net) {
    net.Write<Net::PacketType>(Net::PacketType::GetMessages, -1);

    size_t count;
    net.Read<size_t>(count, -1);

    for (size_t i = 0; i < count; ++i) {
        Message message;
        net.Read<Message>(message, -1);

        std::cout << "\033[33m" << "-(" << i + 1 << ")--------------------------" << std::endl;
        std::cout << "From : " << message.From() << std::endl;
        std::cout << "To   : " << message.To() << std::endl;
        std::cout << "Text : " << message.Text() << std::endl;
        std::cout << "Timestamp : " << message.Time() << std::endl;
        std::cout << "\033[0m";
    }

}

void Client::showAllRooms(Net &net) {
    net.Write<Net::PacketType>(Net::PacketType::getNameRooms, -1);

    size_t count;
    net.Read<size_t>(count, -1);
    for (size_t i = 0; i < count; ++i) {
        std::cout << "\033[33m" << "------------------------------" << std::endl;
        User user;
        net.Read<User>(user, -1);

        std::cout << "Name: " << user.Name() << std::endl;
        std::cout << "Link: " << user.Login() << std::endl;
    }
    std::cout << "\033[0m";
}
