#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <list>
#include <vector>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")


std::list<SOCKET> ClientList;
void Run(SOCKET);

int main()
{
    WSADATA data;
    int e = WSAStartup(MAKEWORD(2, 2), &data);
    if (e != 0) {
        std::cout << "";
    }
    struct sockaddr_in addr;
    addr.sin_port = htons(8080);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        std::cout << "socket error\n";
        return 1;
    }

    int result = connect(sock, (sockaddr*)(&addr), sizeof(addr));
    if (result < 0)
    {
        std::cout << "connct error\n";
        return 1;
    }
    send(sock, "ok", 3, 0);
    return 0;
}
