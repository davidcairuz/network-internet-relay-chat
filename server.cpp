#include "socket.h"

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
        socket->Set_not_error();
        return;      
        // exit(1);
    }
}

int main(int argc, char* argv[]){
    Socket* socket = new Socket(DEFAULT_PORT, "0.0.0.0");
    check(socket);

    socket->Bind();
    check(socket);

    socket->Listen();
    check(socket);

    // socket = 
    socket->Accept();
    check(socket);
    
    socket->Write("Type something here...\n");
    check(socket);

    string message = socket->Read();
    cout << message;
    cin >> message;
    
    socket->Write(message);
    check(socket);
    
    delete socket;
    check(socket);

    return 0;
}