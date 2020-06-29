#include "socket.h"
#include <algorithm>
#include <vector>
#include <pthread.h>

Socket* socket_server = new Socket("0.0.0.0");
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class Client {
public:
    int id;
    int conn;
    string name;
    
    int channel_id;
    string channel_name;

    bool is_admin;
    bool is_muted;
    
    Client(int id, int conn, string name, int channel_id, string channel_name, bool is_admin) {
        this->id = id;
        this->conn = conn;
        this->name = name;

        this->channel_id = channel_id;
        this->channel_name = channel_name;

        this->is_admin = is_admin;
        this->is_muted = false;
    }
};

class ActiveClients {
public:
    vector<Client> clients;
    int curr_id = 0;
    ActiveClients(){}

    void insert(int conn, string name, int channel_id, string channel_name, bool is_admin){
        pthread_mutex_lock(&lock);
        Client new_client = Client(curr_id++, conn, name, channel_id, channel_name, is_admin);
        clients.push_back(new_client);
    
        cout << "Active clients: \n";
        for (int i = 0; i < clients.size(); i++) {
            cout << '\t' << clients[i].id << endl;
        }

        pthread_mutex_unlock(&lock);
    }

    int find(int conn){
        int pos = 0;
        for(; pos < (int)clients.size(); pos++) 
            if(clients[pos].conn == conn) break;
        return pos;
    }

    void remove(int conn) {
        pthread_mutex_lock(&lock);
    
        int pos = find(conn);

        clients.erase(clients.begin() + pos);

        cout << " Active clients: \n";
        for (int i = 0; i < clients.size(); i++) {
            cout << '\t' << clients[i].id << endl;
        }
        
        pthread_mutex_unlock(&lock);
    }

};

ActiveClients* active = new ActiveClients();

// Manda mensagem para todos os clientes
// Conectados
void spread_message(string message, int speaker) {
    pthread_mutex_lock(&lock);
    
    string name;
    
    auto pos = active->find(speaker);
    name = active->clients[pos].name + "#" + to_string(active->clients[pos].id) + ": ";

    int max_len = Socket::buffer_size - name.size();
    vector<int> to_be_removed;

    for (Client client : active->clients) {
        int conn = client.conn;

        for (int i = 0; (unsigned int)i < message.size(); i += max_len) {
            string actual_message = name + message.substr(i, max_len);
            
            int status = -1, retries = Socket::max_retries;
            while (status == -1 && retries--)
                status = socket_server->Write(actual_message, conn);

            if (status == -1) to_be_removed.push_back(conn);
        }
    }

    for (int client : to_be_removed) active->remove(client);
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
    
    if (status == -1) active->remove(speaker);
    
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
            active->remove(new_client);
            quit = true;

        } else if (message == "/ping") {
            send_message("pong!", new_client);

        } else if(message.size() >= 9 && message.substr(0, 9) == "/nickname") {
            string new_nickname = message.substr(10, message.size());
            auto pos = active->find(new_client);
            active->clients[pos].name = new_nickname;
        
        } else {
            spread_message(message, new_client);
        }        
    }

    cout << "A user has left us :(" << endl;
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
        
        if (active->clients.size() == Socket::max_clients) {
            socket_server->Write("Too many people here, you are not welcome\n");
            continue;
        } 

        string nickname = socket_server->Read(new_client);
        cout << "New client: " << nickname << endl;
        active->insert(new_client, nickname, 0, " ", false);
        cout << "Done" << endl;

        if (pthread_create(&tid, NULL, server_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;
    }

    return 0;
}