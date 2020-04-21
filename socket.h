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
#define log 0

using namespace std;

class Socket {
private:
    int port;
    string ip;
    string name;
    int sock_fd;
    int conn_fd;
    bool connected = false;
    struct sockaddr_in serv_addr, client_addr;
    Error error;

public:
    static const int buffer_size = 100;
    Socket(string ip, string name="server", int port=DEFAULT_PORT);
    ~Socket();
    void Bind();
    void Listen();
    void Connect();
    void Accept();
    void Disconnect();
    string Read();
    void Write(string msg);

    string Get_error();
    bool Check_error();
    void Set_not_error();
};

#endif