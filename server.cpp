#include "socket.h"
#include <algorithm>
#include <vector>
#include <pthread.h>

Socket* socket_server = new Socket("0.0.0.0", "1");
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int active_clients = 0;
bool quit = false;
vector<int> clients;

// void Sigint_handler(int sig_num) {
//     signal(SIGINT, Sigint_handler);
//     cout << "\nCannot quit using crtl+c\n";
//     fflush(stdout);
// }

void spread_message(string message, int speaker) {
    pthread_mutex_lock(&lock);

    for (int client : clients) {
        if (client == speaker) continue;
        
        for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
            socket_server->Write(message.substr(i, Socket::buffer_size), client);
            socket_server->Check();
        }
    }

    pthread_mutex_unlock(&lock);
}

void* server_thread(void* arg) {
    int new_client = *((int*)arg);

    spread_message("A new client has joined us!", new_client);

    while (!quit) {
        string message = socket_server->Read(new_client);
        socket_server->Check();
        spread_message(message, new_client);
    }

    cout << "Exit socket_thread" << endl;
    close(new_client);
    pthread_exit(NULL);    
}

void insert_client(int new_client) {
    pthread_mutex_lock(&lock);
    clients.push_back(new_client);
    pthread_mutex_unlock(&lock);
}

void remove_client(int client) {
    pthread_mutex_lock(&lock);
    auto pos = find(clients.begin(), clients.end(), client);
    clients.erase(pos);
    pthread_mutex_unlock(&lock);
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
        
        if (active_clients == Socket::max_clients) {
            socket_server->Write("Too many people here, you are not welcome\n");
            close(new_client);
            continue;
        } 

        insert_client(new_client);

        if (pthread_create(&tid[num_threads], NULL, server_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;
    }

    return 0;
}