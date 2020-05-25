#include "socket.h"
#include <algorithm>
#include <pthread.h>

Socket* socket_server = new Socket("0.0.0.0", "1");
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// void Sigint_handler(int sig_num) {
//     signal(SIGINT, Sigint_handler);
//     cout << "\nCannot quit using crtl+c\n";
//     fflush(stdout);
// }

void* socket_thread(void* arg) {
    int new_client = *((int*)arg);    
    pthread_mutex_lock(&lock);

    /* Garante que todas as mensagens enviadas serão recebidas */
    string message = "";
    do {
        message = socket_server->Read(new_client);
        socket_server->Check();
        
        cout << new_client << ": " << message << "\n";
    } while (message.size() == Socket::buffer_size);

    pthread_mutex_unlock(&lock);
    sleep(2);

    /* Espera o usuário digitar uma mensagem válida para enviar */
    do {
        cout << "To client: ";
        getline(cin, message, '\n');
    } while (message.size() == 0); 

    /* Divide mensagens com mais de "buffer_size" caracteres e as envia em sequência */
    for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        socket_server->Write(message.substr(i, Socket::buffer_size));
        socket_server->Check();
    }

    cout << "Exit socket_thread" << endl;
    close(new_client);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // signal(SIGINT, Sigint_handler);
    socket_server->Check();

    socket_server->Bind();
    socket_server->Check();

    cout << "Chat Initialized =D\n";

    socket_server->Listen();
    socket_server->Check();

    pthread_t tid[Socket::max_clients];
    int num_threads = 0;

    while (1) {
        int new_client = socket_server->Accept();
        socket_server->Check();        
        
        if (pthread_create(&tid[num_threads], NULL, socket_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;

        if (num_threads >= Socket::max_clients * 0.9) {
            num_threads = 0;
            while (num_threads < Socket::max_clients * 0.9) pthread_join(tid[num_threads++], NULL);
            num_threads = 0;
        }
    }

    return 0;
}