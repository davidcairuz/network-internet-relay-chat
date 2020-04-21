#include "socket.h"
#include <algorithm>

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
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
    
    socket->Write("Type something here: ");
    check(socket);

    string message = "";
    while (true) {
        message = socket->Read();
        cout << "From client: " << message << "\n";
        cout << "To client: ";
        getline(cin, message, '\n');
    
        socket->Write(message);
        check(socket);

        if (message == "/quit") break;
    }

    delete socket;
    check(socket);

    return 0;
}