#include "socket.h"
#include <algorithm>

int main(int argc, char* argv[]) {

    Socket* socket = new Socket(LOCALHOST);
    socket->check();

    socket->Connect();
    socket->check();
    
    string message = "";

    while (message != "/quit") {

        do {
            message = socket->Read();
            socket->check();

            cout << "From server: " << message << "\n";
        } while (message.size() == Socket::buffer_size);

        if (message == "/quit") break;

        cout << "To Server: ";
        getline(cin, message, '\n');

        if (message == "/quit") break;

        for (int i = 0; i < message.size(); i += Socket::buffer_size) {
            socket->Write(message.substr(i, Socket::buffer_size));
            socket->check();
        }
    }
    
    delete socket;
    return 0;
}