#include "socket.h"
#include <algorithm>
#include <vector>
#include <pthread.h>

Socket* socket_server = new Socket("0.0.0.0");
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int active_clients = 0;
vector<int> clients;
vector<pair <int, string>> ids;
int curr_id = 0;

// Inesere o file descriptor e
// nome de usuário do cliente em ids
void insert_client(int new_client, string nickname) {
    pthread_mutex_lock(&lock);
    
    clients.push_back(new_client);
    ids.push_back({curr_id++, nickname});
    active_clients++;
    
    cout << "Active clients: \n";
    for (int i = 0; i < (int)ids.size(); i++) {
        cout << '\t' << ids[i].second << endl;
    }

    pthread_mutex_unlock(&lock);
}

// Remove o cliente de ids, 
// Liberando espaço para novos usuários
void remove_client(int client) {
    pthread_mutex_lock(&lock);
    
    auto pos = find(clients.begin(), clients.end(), client) - clients.begin();
    cout << pos << endl;
    ids.erase(ids.begin() + pos);
    clients.erase(clients.begin() + pos);
    active_clients--;

    cout << clients.size() << " " << ids.size() << " Active clients: \n";
    for (int i = 0; i < (int)ids.size(); i++) {
        cout << '\t' << ids[i].first << " " << ids[i].second << " " << clients[i] << endl;
    }
     
    pthread_mutex_unlock(&lock);
}

// Manda mensagem para todos os clientes
// Conectados
void spread_message(string message, int speaker) {
    pthread_mutex_lock(&lock);
    
    string name;
    
    auto pos = find(clients.begin(), clients.end(), speaker) - clients.begin();
    name = ids[pos].second + "#" + to_string(ids[pos].first) + ": ";

    int max_len = Socket::buffer_size - name.size();
    vector<int> to_be_removed;

    for (int client : clients) {
        if (client == speaker) continue;

        for (int i = 0; (unsigned int)i < message.size(); i += max_len) {
            string actual_message = name + message.substr(i, max_len);
            
            int status = -1, retries = Socket::max_retries;
            while (status == -1 && retries--)
                status = socket_server->Write(actual_message, client);

            if (status == -1) to_be_removed.push_back(client);
        }
    }

    for (int client : to_be_removed) remove_client(client);
    pthread_mutex_unlock(&lock);
}

// Manda mensagem para apenas o cliente com
// File descriptor speaker
void send_message(string message, int speaker) {
    pthread_mutex_lock(&lock);

    int status = -1, retries = Socket::max_retries;
    for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        while (status == -1 && retries--)
            status = socket_server->Write(message.substr(i, Socket::buffer_size), speaker);        
    }
    
    if (status == -1) remove_client(speaker);
    
    pthread_mutex_unlock(&lock);
}

// Thread para iteragir com o cliente
void* server_thread(void* arg) {    
    bool quit = false;
    int new_client = *((int*)arg);
    spread_message("A new client has joined us!", new_client);

    while (!quit) {
        string message = socket_server->Read(new_client);
        socket_server->Check();

        if (message == "/quit" || message.empty()) {
            spread_message("Someone left the server", new_client);
            remove_client(new_client);
            quit = true;

        } else if (message == "/ping") {
            send_message("pong!", new_client);

        } else if(message.size() >= 9 && message.substr(0, 9) == "/nickname") {
            string new_nickname = message.substr(10, message.size());
            auto pos = find(clients.begin(), clients.end(), new_client) - clients.begin();
            ids[pos].second = new_nickname;
        
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
        cout << "New client: " << nickname << endl;
        insert_client(new_client, nickname);
        cout << "Done" << endl;

        if (pthread_create(&tid, NULL, server_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;
    }

    return 0;
}