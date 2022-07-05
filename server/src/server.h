#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <errno.h>
#include <fcntl.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <ifaddrs.h>
#endif
class server
{

private:
#ifdef _WIN32
	WSAData wsa;
	char yes = '1';
#endif
#ifdef __unix__
	struct sigaction sa;
	int yes = 1;
	void static sigchld_handler(int s);
#endif
	struct addrinfo hints, *serverinfo;
	struct sockaddr_in *ipAddr;
	int addrLen, sockfd, inet_type, sock_type, sock_protocol, backLog;
	struct sockaddr_storage theirAddr;
	std::string ip, port;
	bool sendTo(int &ockto, std::string msg);
	int recvFrom(int &sockFrom, std::string& msg);
	void *get_in_addr(struct sockaddr *sa);
	std::vector<std::string> pNames;//players' nicks
	std::vector<int> pSocks;//players' socketfile descriptors
	
public:
	server(std::string _port, int _backLog);
#ifdef _WIN32
	bool initWSA();
#endif
	bool bindDefault();
	bool startServer();
	void gameLoop();
	~server();
};
