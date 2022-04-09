#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <list>
#include <vector>
#include <WS2tcpip.h>
#include <locale.h>
#include <Math.h>
#pragma comment(lib, "Ws2_32.lib")

struct Bill {
    int Client;
    double Balance = 0;
    int Number;
};

struct Client
{
    SOCKET Socket;
    char Name[32];
    int ID;
    std::list<Bill*> BillList;
    Client(SOCKET s, int id) {
        Socket = s;
        ID = id;
    }
};

enum OPERATION {
    OPEN, CLOSE, UP, DOWN, UPLEFT
};

struct Data {
    int ID;
    int BillNumber;
    double Value;
    OPERATION Operation;
    char Name[32];
    char exeption[100];
};
int BillNumber = 1;
std::list<Client*> ClientList;
void Run(Client*);

int main()
{
    WSADATA data;
    int e = WSAStartup(MAKEWORD(2, 2), &data);
    if (e != 0) {
        std::cout << "";
        return 1;
    }
    SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == 0) {
        std::cout << "Socket error1" << std::endl;
        return 1;
    }
    sockaddr_in serverAdress;
    memset(&serverAdress, 0, sizeof(serverAdress));
    serverAdress.sin_family = PF_INET;
    serverAdress.sin_port = htons(8080);
    serverAdress.sin_addr.S_un.S_un_b.s_b1 = 127;
    serverAdress.sin_addr.S_un.S_un_b.s_b2 = 0;
    serverAdress.sin_addr.S_un.S_un_b.s_b3 = 0;
    serverAdress.sin_addr.S_un.S_un_b.s_b4 = 1;
    e = bind(serverSocket, (sockaddr*)(&serverAdress), sizeof(serverAdress));
    if (e ==SOCKET_ERROR) {
        std::cout << "bind socket error" << std::endl;
        return 1;
    }
    e = listen(serverSocket, 100);
    if (e < 0) {
        std::cout << "listen socket error" << std::endl;
        return 1;
    };
    int id = 1;
    while (true)
    {
        std::cout << "wait...GO";
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        Client* client = new Client(clientSocket, id++);
        new std::thread(Run, client);
        ClientList.push_back(client);
    }
    return 0;
}

void Run(Client *client) {
    setlocale(LC_ALL, "Russian");
    int n = 0;
    Data recvDate;
    while (true)
    {
        n = recv(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0);
        if (n < 0) {
            return;
        }
        switch (recvDate.Operation)
        {
        case OPERATION::OPEN: {
            Bill* bill = new Bill();
            bill->Balance = 0;
            bill->Client = client->ID;
            bill->Number = BillNumber++;
            client->BillList.push_back(bill);

            recvDate.BillNumber = bill->Number;
            send(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0);
        }
            break;
        case OPERATION::CLOSE: {
            std::list<Bill*>::iterator itr = client->BillList.begin();
            while (itr != client->BillList.end()) {
                if (recvDate.BillNumber == (*itr)->Number) {
                    client->BillList.remove(*itr);
                    break;
                }
                itr++;
            }

        }
            break;
        case OPERATION::UP: {
            std::list<Bill*>::iterator itr = client->BillList.begin();
            while (itr != client->BillList.end()) {
                if (recvDate.BillNumber == (*itr)->Number) {
                   (*itr)->Balance += recvDate.Value;
                   recvDate.Value = (*itr)->Balance;

                    break;
                }
                itr++;
                
            }
            send(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0); }
        break;
           
        
        case OPERATION::DOWN: {
            recvDate.exeption == "";
            std::list<Bill*>::iterator itr = client->BillList.begin();
            while (itr != client->BillList.end()) {
                if (recvDate.BillNumber == (*itr)->Number) {
                    if ((*itr)->Balance < recvDate.Value) {
                        recvDate.Value = (*itr)->Balance;
                        strcpy_s(recvDate.exeption, "На счете недостаточно средств\n");
                        break;
                    }
                    else{
                    (*itr)->Balance -= recvDate.Value;
                    recvDate.Value = (*itr)->Balance;
                    break;
                    }
                }
                itr++;
            }
            send(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0);
        }
        break;
           
        case OPERATION::UPLEFT: {
            std::list<Bill*>::iterator itr = client->BillList.begin();
            while (itr != client->BillList.end()) {
                send(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0);
                itr++;
            }
            //send(client->Socket, (char*)(&recvDate), sizeof(recvDate), 0);
        }
        break;
        default:{

        }
            break;
        }
    }
    ClientList.remove(client);
}
//Dedmix DeLincs Ben 