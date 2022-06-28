#include "client.h"
#include "../../Tic-Tac-Toe/src/board.h"

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
    board table;
    int curSign = 0;
    while(!table.nIsFinished) {
        table.print();
        if(curPlayer) {
            std::cout << "Enter your move: ";
            int mv;
            std::cin >> mv;
            msg = to_string(mv);
            if(cl->sendMessage(msg)) {
                std::cerr << "[ERROR]: send move\n";
                return 1;
            }
            mv--;
            if(cl->recvMessage(msg)) {
                std::cerr << "[ERROR]: recv msg\n";
                return 1;
            }
            if(msg == "OK") {
                assert(table.CheckMove(mv));
                table.Set(mv, curSign);
            }else {
                std::cout << "Incorrect move!!! Re enter your move pleas!!!";
                system("pause");
                continue;
            }
        }else {
            std::cout << "Wait while your oponent makes move...\n";
            while(cl->recvMessage(msg));
            int mv = stoi(msg);
            assert(table.CheckMove(mv - 1));
            table.Set(mv - 1, curSign); 
        }
        curPlayer ^= 1;
        curSign ^= 1;
    }

    if(cl->recvMessage(msg)) {
        std::cerr << "[ERROR]: results\n";
        return 1;
    }
    std::cout << msg << '\n';
    return 0;
}