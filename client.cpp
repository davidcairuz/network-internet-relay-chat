#include "socket.h"
#include <algorithm>

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
        exit(1);
    }
}

int main(int argc, char* argv[]) {

    Socket* socket = new Socket(LOCALHOST);
    check(socket);

    socket->Connect();
    check(socket);
    
    string message = "";

    while (true) {
        string message = socket->Read();
        if (message == "/quit") break;        

        cout << "From Server: " << message << "\n";
        cout << "To Server: ";
        getline(cin, message, '\n');
        
        if (message == "/quit") break;

        socket->Write(message);
        check(socket);
    }
    
    delete socket;
    check(socket);

    return 0;
}