cmake_minimum_required(VERSION 3.22)
project(Console_chat_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH "~/Qt/6.2.4/gcc_64/lib/cmake/")
include_directories(Common Server Client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SERVER_SOURCES
        Server/server_main.cpp
        Server/server.cpp Server/server.h
        Common/Net.cpp Common/Net.h
        Common/User.cpp Common/User.h
        Common/Message.cpp Common/Message.h
        Server/Database.cpp Server/Database.h)

set(CLIENT_SOURCES
        Client/client_main.cpp
        Common/Net.cpp Common/Net.h
        Common/User.cpp Common/User.h
        Common/Message.cpp Common/Message.h Client/Client.cpp Client/Client.h)

find_package(Qt6 COMPONENTS
        Sql
        REQUIRED)


add_executable(Server ${SERVER_SOURCES})
target_link_libraries(Server
        Qt6::Sql
        )

add_executable(Client ${CLIENT_SOURCES})
target_link_libraries(Client
        Qt6::Sql
        )

