#include <iostream>
#include <string>
#include "server.h"

int main(int argc, char *argv[]) {


	std::string port = "1337";
	int backLog = 10;
	if(argc >= 2) {
		port = argv[1];
	}
	if(argc >= 3) {
		backLog = std::stoi(argv[2]);
	}

	server *srv = new server(port, backLog);
#ifdef _WIN32
	srv->initWSA();
#endif	
	srv->bindDefault();
	srv->startServer();
	srv->gameLoop();
	return 0;
}
