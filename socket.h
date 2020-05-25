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
    Error error;
    int sock_fd;
    int conn_fd;
    bool connected;
    struct sockaddr_in serv_addr, client_addr;

public:
    static const int buffer_size = 4096;
    static const int max_size = 65536;
    static const int max_clients = 64;

    /*
     * Construtor do Socket
     * 
     * @param ip: ip utilizado para crirar o socket 
     * @param name: nome do cliente
     * @param port: porta na qual será criada o socket
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
    int Accept();
    
    /*
     * Wrapper da função "disconnect" de C
     */
    void Disconnect();
    
    /*
     * Recebe mensagem de outro socket
     * 
     * @return mensagem recebida
     */
    string Read(int conn_fd);
    
    /*
     * Envia uma mensagem para outro socket
     * 
     * @params msg: mensagem enviada
     */
    void Write(string msg);

    int Get_conn_fd();

    /*
     * Getter da mensagem de erro
     * 
     * @return mensagem de erro
     */
    string Get_error();
    
    /*
     * Getter do status do erro
     */
    bool Has_error();

    /*
     * Checa se há um erro e, caso haja, encerra o programa 
     */
    void Check();
};

#endif