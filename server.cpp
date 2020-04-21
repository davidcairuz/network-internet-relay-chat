#include "socket.h"
#include <algorithm>

int main(int argc, char* argv[]){
    Socket* socket = new Socket("0.0.0.0", "1");
    socket->check();

    socket->Bind();
    socket->check();

    cout << "Chat Initialized =D\n";

    socket->Listen();
    socket->check();

    socket->Accept();
    socket->check();
    
    socket->Write("Type something: ");
    socket->check();
    
    string message = "";
    while (message != "/quit") {
        
        do {
            message = socket->Read();
            socket->check();
            
            cout << "From client: " << message << "\n";
        } while (message.size() == Socket::buffer_size);

        if (message.size() == 0) break;        

        cout << "To client: ";
        getline(cin, message, '\n');

        if (message == "/quit") break;

        for (int i = 0; i < message.size(); i += Socket::buffer_size) {
            socket->Write(message.substr(i, Socket::buffer_size));
            socket->check();
        }
    }

    delete socket;
    cout << "Bye o/\n";

    return 0;
}