#include "socket.h"
#include <algorithm>

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
        delete socket;
        exit(1);
    }
}

int main(int argc, char* argv[]){
    Socket* socket = new Socket("0.0.0.0", "1");
    check(socket);

    socket->Bind();
    check(socket);

    cout << "Chat Initialized =D\n";

    socket->Listen();
    check(socket);

    socket->Accept();
    check(socket);
    
    socket->Write("Type something: ");
    check(socket);

    string message = "";
    while (message != "/quit") {
        
        do {
            message = socket->Read();
            cout << "From client: " << message << "\n";
        } while (message.size() == Socket::buffer_size);

        if (message.size() == 0) break;        

        cout << "To client: ";
        getline(cin, message, '\n');

        if (message == "/quit") break;

        for (int i = 0; i < message.size(); i += Socket::buffer_size) {
            socket->Write(message.substr(i, Socket::buffer_size));
            check(socket);
        }
    }

    delete socket;
    cout << "Bye o/\n";

    return 0;
}