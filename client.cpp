#include "socket.h"
#include <algorithm>
#include <pthread.h>

// void Sigint_handler(int sig_num) {
//     signal(SIGINT, Sigint_handler);
//     cout << "\nCannot quit using crtl+c\n";
//     fflush(stdout); 
// }

void* client_thread(void* arg) {
    cout << "Connected to server" << endl;
    Socket* socket = new Socket(LOCALHOST);
    socket->Check();

    socket->Connect();
    socket->Check();

    string message = "";
    /* Espera o usuário digitar uma mensagem válida para enviar */
    do {
        cout << "To Server: ";
        getline(cin, message, '\n');
    } while (message == ""); 

    if (message == "/quit") {
        delete socket;
        pthread_exit(NULL);
    }

    /*Divide mensagens com mais de "buffer_size" caracteres e as envia em sequência*/
    for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        socket->Write(message.substr(i, Socket::buffer_size));
        socket->Check();
    }

    /* Garante que todas as mensagens enviadas serão recebidas */        
    do {
        message = socket->Read(socket->Get_conn_fd());
        socket->Check();

        cout << "From server: " << message << "\n";
    } while (message.size() == Socket::buffer_size);

    delete socket;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // signal(SIGINT, Sigint_handler); 

    pthread_t tid;

    string command = "";

    while (command != "/connect") getline(cin, command);

    if (pthread_create(&tid, NULL, client_thread, NULL) != 0) {
        cout << "Failed to create thread" << endl;
    }

    pthread_join(tid, NULL);
    
    return 0;
}