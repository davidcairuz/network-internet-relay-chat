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
    static const int buffer_size = 4096;
    
    /*
     * Construtor do Socket
     */
    Socket(string ip, string name="server", int port=DEFAULT_PORT);
    ~Socket();

    /*
     * Wrapper da função "bind" de C
     */
    void Bind();

    /*
     * Wrapper da função "listen" de C
     */
    void Listen();

    /*
     * Wrapper da função "connect" de C
     */
    void Connect();
    
    /*
     * Wrapper da função "accept" de C
     */
    void Accept();
    
    /*
     * Wrapper da função "disconnect" de C
     */
    void Disconnect();
    
    /*
     * Wrapper da função "read" de C
     */
    string Read();
    
    /*
     * Wrapper da função "write" de C
     */
    void Write(string msg);

    /*
     * Getter da mensagem de erro
     */
    string Get_error();
    
    /*
     * Getter do status do erro
     */
    bool has_error();

    /*
     * Encerra o programa caso exista um erro 
     */
    void check();
};

#endif