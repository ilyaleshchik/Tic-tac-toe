#include "server.h"
#include "../../Tic-Tac-Toe/src/board.h"
#include <cassert>
#include <string>

server::server(std::string _port, int _backLog) {
	port = _port;
	inet_type = sock_type = -1;
	ip = "-1";
	backLog = _backLog;
}

#ifdef _WIN32
bool server::initWSA() {

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	return 0;
}
#endif

#ifdef __unix__
void server::sigchld_handler(int s) {

	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
	return;
}
#endif

void *server::get_in_addr(struct sockaddr* sa) {
	if(sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

bool server::bindDefault() {

	freeaddrinfo(serverinfo);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int rv = getaddrinfo(nullptr, port.c_str(), &hints, &serverinfo);
	if(rv != 0) {
		std::cerr << "[ERROR]: GetAddrinfo\n";
		freeaddrinfo(serverinfo);
		return 1;
	}
	struct addrinfo *p;
	for(p = serverinfo; p != NULL; p = p->ai_next) {
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			freeaddrinfo(serverinfo);
			return 0;
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
#ifdef __unix__
			close(sockfd);
#else
			closesocket(sockfd);
#endif
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(serverinfo);
	if(p == NULL) {
		return 1;
	}
	return 0;
}


bool server::sendTo(int &sockto, std::string msg) {
	return (send(sockto, msg.c_str(), msg.size(), 0) == -1);
}


bool server::startServer() {

	if(listen(sockfd, backLog) == -1) {
		return 1;
	}
#ifdef __unix__
	sa.sa_handler = server::sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		std::cerr << "[ERROR]: sigaction";
		return 1;
	}
#endif

	std::cerr << "[SERVER]: waiting for connections...\n";

	while(pSocks.size() != 2) {///wait while two players connected
		socklen_t addr_size = sizeof theirAddr;
		int newfd = accept(sockfd, (struct sockaddr *)&theirAddr, &addr_size);//accept new connection
		if(newfd < 0) {
			std::cerr << "[ERROR]: accept()\n";
			continue;
		}
		char curIp[INET_ADDRSTRLEN];
		inet_ntop(theirAddr.ss_family, get_in_addr((struct sockaddr *)&theirAddr), curIp, sizeof curIp);
		std::cerr << "[CONNECTION]: " << curIp << '\n';
		std::string msg;
		if(recvFrom(newfd, msg) <= 0) {//get new player's name
			std::cerr << "[ERROR]: recvFrom()";
			close(newfd);
			continue;
		}
		pSocks.push_back(std::move(newfd));
		pNames.push_back(msg);
	}
	return 0;
}

int server::recvFrom(int &sockFrom, std::string& msg) {
	char buf[100];

	int numbytes = recv(sockFrom, buf, 99, 0);
	if(numbytes == -1) {
		return -1;
	}

	buf[numbytes] = '\0';
	msg = buf;
	return numbytes;
}


void server::gameLoop() {

	std::srand(time(NULL));
	int curPlayer = std::rand() % 2;

//---------send players which turn now-------
	if(sendTo(pSocks[curPlayer], "1")) {
		std::cerr << "[DISCONNECT]: Payer " << pNames[curPlayer] << " has disconnected\n";
		close(pSocks[curPlayer]);
		//send second player win message
		return;
	}
	if(sendTo(pSocks[curPlayer ^ 1], "0")) {
		std::cerr << "[DISCONNECT]: Payer " << pNames[curPlayer ^ 1] << " has disconnected\n";
		close(pSocks[curPlayer ^ 1]);
		//send second player win message
		return;
	}


	std::string msg;
	board table;
	int curSign = 0;
	while(!table.nIsFinished) {//while game won't end or someone will win
		while(recvFrom(pSocks[curPlayer], msg) <= 0); //recv message from current player
		std::cerr << "[MOVE]: " << pNames[curPlayer] << " - " << msg << '\n';
		int curMove = stoi(msg);
		curMove--;
		if(table.CheckMove(curMove)) { //cheching current move
			if(sendTo(pSocks[curPlayer], "OK")) {
				std::cerr << "[DISC]: Player " << pNames[curPlayer] << " has disconeccted!\n";
				///send other player win message
				close(pSocks[curPlayer]);
				return; 
			}
			table.Set(curMove, curSign);
		}else { //if it's wrong send current player [WRONG] message
			if(sendTo(pSocks[curPlayer], "WR")) {
				std::cerr << "[DISC]: Player " << pNames[curPlayer] << " has disconeccted!\n";
				///send other player win message
				close(pSocks[curPlayer]);
				return; 
			}
			continue;
		}
		sleep(0.1);
		if(sendTo(pSocks[curPlayer ^ 1], msg)) { //send other player current move
			std::cerr << "[DISC]: Player " << pNames[curPlayer ^ 1] << " has disconeccted!\n";
			///send other player win message
			close(pSocks[curPlayer ^ 1]);
			return; 
		}

		//switch players
		curPlayer ^=  1;
		curSign ^= 1;
	}

	std::string res1 = "TIE", res2 = "TIE";
	if(table.nIsFinished == 1) {
		res1 = "WIN";
		res2 = "LOSE";

	}

//----------sedning results------------------
	if(sendTo(pSocks[curPlayer ^ 1], res1)) {
		std::cerr << "[ERROR]: send res1\n";
		close(pSocks[curPlayer ^ 1]); 
	}
	sleep(0.1);
	if(sendTo(pSocks[curPlayer], res2)) {
		std::cerr << "[ERROR]: send res2\n"; 
		close(pSocks[curPlayer]);
	}
	return;
}

 
server::~server() {
	delete serverinfo;
	for(int &i: pSocks) {
		close(i);
	}
#ifdef _WIN32
	WSACleanup();
#endif
}
