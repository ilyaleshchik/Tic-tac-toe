#include "client.h"

int main(int argc, char *argv[]) {



    std::string pName;
    std::cout << "Enter player name: ";
    std::cin >> pName;


    std::string ip, port;

    if(argc != 3) {
        ip = "127.0.0.1";
        port = "1337";
    }else {
        ip = argv[1];
        port = argv[2];
    }

    client *cl = new client(ip, port);

#ifdef _WIN32
    cl->initWSA();
#endif
    int ret = cl->connectServer();

    if(ret != 0) 
        return 1;

    if(cl->sendMessage(pName)) {
        std::cerr << "[ERROR]: send name\n";
        return 1;
    }

    std::string msg;
    if(cl->recvMessage(msg)) {
        std::cerr << "[ERROR]: get cur player\n";
        return 1;
    }

    int curPlayer = stoi(msg);
    std::cerr << "[PLAYER]: " << curPlayer << '\n';

    if(curPlayer) {
        std::cin >> msg;
        if(cl->sendMessage(msg)) {
            std::cerr << "[ERROR]: sendMessage()\n";
            return 1;
        }
    }
    return 0;
}