cmake_minimum_required(VERSION 3.6)
project(Socket_IO_Server_CPP)

include_directories($ENV{HOME}/websocketCPP/websocketpp $ENV{HOME}/rapidjson/rapidjson/include)

set(CMAKE_CXX_STANDARD 14)
set(Boost_USE_STATIC_LIBS, ON)
set(Boost_INCLUDE_DIR /usr/local/include)
set(Boost_LIBRARY_DIR /usr/local/lib)

find_package(Boost COMPONENTS system filesystem date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(SOURCE_FILES
        Socket.IO.Server/SioEvent.cpp
        Socket.IO.Server/SioEvent.h
        Socket.IO.Server/SioEventAdapter.h
        Socket.IO.Server/Socket.IO.Server.cpp
        Socket.IO.Server/SocketIOClientHandler.h
        Socket.IO.Server/SocketIOMessage.h
        Socket.IO.Server/SocketIOPacket.cpp
        Socket.IO.Server/SocketIOPacket.h
        Socket.IO.Server/stdafx.cpp
        Socket.IO.Server/stdafx.h
        Socket.IO.Server/targetver.h
        Socket.IO.Server/WebSocketServer.h)

add_executable(Socket_IO_Server_CPP ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})