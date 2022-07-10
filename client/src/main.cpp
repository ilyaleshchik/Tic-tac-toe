#include "client.h"
#include "../../Tic-Tac-Toe/src/board.h"

int main(int argc, char *argv[]) {



    std::string pName;
    std::cout << "Enter player name: ";
    std::cin >> pName;

    std::string ip = "127.0.0.1", port = "1337";

    if(argc == 3) {
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

    cl->gameLoop(pName);
    return 0;
}