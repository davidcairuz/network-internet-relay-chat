#include "socket.h"

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    Socket* socket = new Socket(DEFAULT_PORT, LOCALHOST);
    check(socket);

    socket->Connect();
    check(socket);
    
    string message = socket->Read();
    cin >> message;

    socket->Write(message);
    check(socket);
    
    delete socket;
    check(socket);

    return 0;
}