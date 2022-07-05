#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <errno.h>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include "../../Tic-Tac-Toe/src/board.h"

//Windows includes
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

//Linux and maxOS includes
#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <ifaddrs.h>
#endif


class client {

private:

#ifdef _WIN32
    WSAData wsa;
#endif
    struct addrinfo hints, *res;
    int sockfd;
    std::string ip, port;
    void *getInAddr(struct sockaddr *sa);
    bool initSocket();//init socket descriptor

public:
    client(std::string _ip, std::string _port);
#ifdef _WIN32
    bool initWSA();
#endif
    bool connectServer();
    bool recvMessage(std::string &msg);//reciev text message
    bool sendMessage(std::string &msg);
    void gameLoop(std::string &pName);
    ~client();
};