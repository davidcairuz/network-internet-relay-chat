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

        do {
            message = socket->Read();
            cout << "From server: " << message << "\n";
        } while (message.size() == Socket::buffer_size);

        if (message == "/quit") break;

        cout << "To Server: ";
        getline(cin, message, '\n');

        if (message == "/quit") break;

        for (int i = 0; i < message.size(); i += Socket::buffer_size) {
            socket->Write(message.substr(i, Socket::buffer_size));
            check(socket);
        }
    }
    
    delete socket;
    check(socket);

    return 0;
}