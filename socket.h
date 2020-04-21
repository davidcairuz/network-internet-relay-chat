#ifndef SOCKET_H
#define SOCKET_Hh

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/signal.h>
#include <string>
#include <string.h>
#include "error.h"

#define LOCALHOST "127.0.0.1"
#define DEFAULT_PORT 8080

using namespace std;

class Socket {
private:
    int port;
    string ip;
    int sock_fd;
    int conn_fd;
    bool connected = false;
    struct sockaddr_in serv_addr, client_addr;
    static const int buffer_size = 4096;
    Error error;

public:
    Socket(int port, string ip);
    Socket(int sock_fd, sockaddr_in serv_addr);
    ~Socket();
    
    void Bind();
    void Listen();
    void Connect();
    Socket* Accept();
    void Disconnect();
    string Read();
    void Write(string msg);

    string Get_error();
    bool Check_error();
};

#endif