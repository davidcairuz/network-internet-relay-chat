#include "socket.h"

void check(Socket* socket) {
    if (socket->Check_error()) {
        cout << socket->Get_error();
        return;      
        // exit(1);
    }
}

int main(int argc, char* argv[]){
    Socket* socket = new Socket(DEFAULT_PORT, "0.0.0.0");
    check(socket);
    
    socket->Connect();
    check(socket);

    cout << "aaa\n";
    socket->Bind();
    cout << "bbb\n";

    check(socket);
    cout << "ccc\n";
    socket->Write("Type something here...\n");
    check(socket);
    
    socket->Listen();
    check(socket);

    socket->Accept();
    check(socket);
    
    string message;
    cin >> message;
    
    socket->Write(message);
    check(socket);
    

    delete socket;
    check(socket);

    return 0;
}