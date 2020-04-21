#include "socket.h"
#include <algorithm>

int main(int argc, char* argv[]) {

    Socket* socket = new Socket(LOCALHOST);
    socket->Check();

    socket->Connect();
    socket->Check();
    
    string message = "";

    while (message != "/quit") {

        /* Garante que todas as mensagens enviadas serão recebidas */        
        do {
            message = socket->Read();
            socket->Check();

            cout << "From server: " << message << "\n";
        } while (message.size() == Socket::buffer_size);

        if (message.size() == 0) break;        

        /* Espera o usuário digitar uma mensagem válida para enviar */
        do {
            cout << "To Server: ";
            getline(cin, message, '\n');
        } while (message.size() == 0); 


        if (message == "/quit") break;

        /*Divide mensagens com mais de "buffer_size" caracteres e as envia em sequência*/
        for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
            socket->Write(message.substr(i, Socket::buffer_size));
            socket->Check();
        }
    }
    
    delete socket;
    return 0;
}