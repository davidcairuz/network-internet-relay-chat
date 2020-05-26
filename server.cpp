#include "socket.h"
#include <algorithm>
#include <vector>
#include <pthread.h>

Socket* socket_server = new Socket("0.0.0.0");
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int active_clients = 0;
bool quit = false;
vector<int> clients;
vector<pair <int, string>> ids;
int curr_id = 0;

void insert_client(int new_client, string nickname) {
    pthread_mutex_lock(&lock);
    
    clients.push_back(new_client);
    ids.push_back({curr_id++, nickname});
    active_clients++;
    
    pthread_mutex_unlock(&lock);
}

void remove_client(int client) {
    pthread_mutex_lock(&lock);
    
    auto pos = find(clients.begin(), clients.end(), client);
    clients.erase(pos);
    ids.erase((pos - clients.begin()) + ids.begin());
    active_clients--;
    
    pthread_mutex_unlock(&lock);
}

void spread_message(string message, int speaker) {
    pthread_mutex_lock(&lock);
    
    auto pos = find(clients.begin(), clients.end(), speaker) - clients.begin();
    string name = to_string(ids[pos].first) + "#" + ids[pos].second + ": ";
    int max_len = Socket::buffer_size - name.size();

    for (int client : clients) {
        if (client == speaker) continue;
        
        for (int i = 0; (unsigned int)i < message.size(); i += max_len) {
            string actual_message = name + message.substr(i, max_len);
            
            int status = -1, retries = Socket::max_retries;
            while (status == -1 && retries--)
                status = socket_server->Write(actual_message, client);
        }
    }

    pthread_mutex_unlock(&lock);
}

void send_message(string message, int speaker) {
    pthread_mutex_lock(&lock);
    
    for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        int status = -1, retries = Socket::max_retries;
        while (status == -1 && retries--)
            status = socket_server->Write(message.substr(i, Socket::buffer_size), speaker);
    }

    pthread_mutex_unlock(&lock);
}

void* server_thread(void* arg) {
    int new_client = *((int*)arg);

    spread_message("A new client has joined us!", new_client);

    while (!quit) {
        string message = socket_server->Read(new_client);
        socket_server->Check();

        if (message.size() == 5 && message.substr(message.size() - 5, 5) == "/quit") {
            spread_message("Someone left the server", new_client);
            remove_client(new_client);
        } else if (message.size() == 5 && message.substr(message.size() - 5, 5) == "/ping") {
            send_message("pong", new_client);
        } else {
            spread_message(message, new_client);
        }
    }

    cout << "Exit socket_thread" << endl;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    socket_server->Bind();
    socket_server->Check();

    cout << "Chat Initialized =D\n";

    socket_server->Listen();
    socket_server->Check();

    pthread_t tid;

    while (1) {
        int new_client = socket_server->Accept();
        socket_server->Check();
        
        if (active_clients == Socket::max_clients) {
            socket_server->Write("Too many people here, you are not welcome\n");
            continue;
        } 

        string nickname = socket_server->Read(new_client);
        insert_client(new_client, nickname);

        if (pthread_create(&tid, NULL, server_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;
    }

    return 0;
}