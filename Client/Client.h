//
// Created by kkorzhanevskii on 23.05.22.
//

#pragma once
#include <vector>
#include "Net.h"
/** Client^ class implementing client application
 *
 */
class Client {
public:

    /*** Star: connect to the server and star main loop.
     *
     * @param host [in] string with an addres of host to connect
     */
    void start(std::string host = "127.0.0.1");

private:
    void showStartMenu(Net& net);
    void showLoginMenu(Net& net);
    void showRegistrationMenu(Net& net);
    void showChatMenu(Net& net);

    void showChat(Net& net);
    void sendMessageToUsr(Net& net);
    void sendMessageToGroup(Net& net);
    void showAllUsers(Net& net);
    void showAllRooms(Net& net);
    void banUsr(Net& net);


    User user_;
};
