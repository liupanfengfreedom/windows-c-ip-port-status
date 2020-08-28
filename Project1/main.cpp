#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <string>     // std::string, std::to_stri
// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
struct IPv4
{
    unsigned char b1, b2, b3, b4;
};

bool getMyIP(IPv4& myIP, std::string& ipstr)
{
    char szBuffer[1024];

#ifdef WIN32
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    if (::WSAStartup(wVersionRequested, &wsaData) != 0)
        return false;
#endif


    if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

    struct hostent* host = gethostbyname(szBuffer);
    if (host == NULL)
    {
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

    //Obtain the computer's IP
    myIP.b1 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1;
    myIP.b2 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2;
    myIP.b3 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3;
    myIP.b4 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
    WSACleanup();
#endif
    ipstr = std::to_string(myIP.b1) + "." + std::to_string(myIP.b2) + "." + std::to_string(myIP.b3) + "." + std::to_string(myIP.b4);
    return true;
}
bool isportopen(int port,bool btcp=true)
{
    int socktype;
    int protocol;
    if (btcp)
    {
        socktype = SOCK_STREAM;
        protocol = IPPROTO_TCP;
    }
    else
    {
        socktype = SOCK_DGRAM;
        protocol = IPPROTO_UDP;
    }
    // Declare some variables
    WSADATA wsaData;

    int iResult = 0;            // used to return function results

    // the listening socket to be created
    SOCKET ListenSocket = INVALID_SOCKET;

    // The socket address to be passed to bind
    sockaddr_in service;

    //----------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"Error at WSAStartup()\n");
        return 1;
    }
    //----------------------
    // Create a SOCKET for listening for 
    // incoming connection requests
   // ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   // ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ListenSocket = socket(AF_INET, socktype, protocol);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    IPv4 ipv4;
    std::string ipstr;
    getMyIP(ipv4, ipstr);
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ipstr.c_str());
    //service.sin_addr.s_addr = inet_addr("192.168.20.96");
    service.sin_port = htons(port);

    //----------------------
    // Bind the socket.
    iResult = bind(ListenSocket, (SOCKADDR*)&service, sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else
        wprintf(L"bind returned success\n");

    WSACleanup();
    return 0;
}


int main()
{
    bool b = isportopen(8085, true);
    
    wprintf(L"result %u\n", b);


}