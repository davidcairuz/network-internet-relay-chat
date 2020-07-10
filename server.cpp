#include "socket.h"
#include <algorithm>
#include <vector>
#include <pthread.h>
#include <map>

Socket* socket_server = new Socket("0.0.0.0", "server", 8192);
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
map<string, int> channels;
map<string, vector<int>> channels_permissions;

class Client {
public:
    int id;
    int conn;
    string ip;
    string name;
    
    int channel_id;
    string channel_name;

    bool is_admin;
    bool is_muted;
    
    Client(int id, int conn, string name, string ip) {
        this->id = id;
        this->ip = ip;
        this->conn = conn;
        this->name = name;

        this->channel_name = "$";
        this->is_admin = false;
        this->is_muted = false;
    }
};

class ActiveClients {
public:
    vector<Client> clients;
    int curr_id = 0;
    ActiveClients(){}

    void insert(int conn, string name, string ip){
        pthread_mutex_lock(&lock);
        Client new_client = Client(curr_id++, conn, name, ip);
        clients.push_back(new_client);
    
        cout << "Active clients: \n";
        for (unsigned int i = 0; i < clients.size(); i++) {
            cout << '\t' << clients[i].id << endl;
        }

        pthread_mutex_unlock(&lock);
    }

    int find(int conn){
        for (unsigned int pos = 0; pos < clients.size(); pos++) 
            if(clients[pos].conn == conn) return pos;
        return -1;
    }

    int find(string name){
        for (unsigned int pos = 0; pos < clients.size(); pos++) 
            if(clients[pos].name == name) return pos;
        return -1;
    }

    int find(string channel, string name){
        for(unsigned int pos = 0; pos < clients.size(); pos++) 
            if(clients[pos].name == name && clients[pos].channel_name == channel) return pos;
        return -1;
    }

    void remove(int conn) {
        pthread_mutex_lock(&lock);
    
        int pos = find(conn);

        clients.erase(clients.begin() + pos);

        cout << " Active clients: \n";
        for (unsigned int i = 0; i < clients.size(); i++) {
            cout << '\t' << clients[i].id << endl;
        }

        pthread_mutex_unlock(&lock);
    }
};

ActiveClients* active = new ActiveClients();

// Manda mensagem para apenas o cliente com
// File descriptor speaker
void send_message(string message, int speaker) {
    int status = -1, retries = Socket::max_retries;
    for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
        cout << speaker << " speaking\n";
        while (status == -1 && retries--)
            status = socket_server->Write(message.substr(i, Socket::buffer_size), speaker);        
    }
    
    if (status == -1) active->remove(speaker);
    
    pthread_mutex_unlock(&lock);
}

// Manda mensagem para todos os clientes
// Conectados
void spread_message(string message, int speaker) {
    pthread_mutex_lock(&lock);
    
    string name;
    
    int pos = active->find(speaker);
    name = active->clients[pos].name + "#" + to_string(active->clients[pos].id) + ": ";
    
    if (active->clients[pos].channel_name == "$") {
        send_message("Please, join a channel", speaker);
        pthread_mutex_unlock(&lock);
        return;
    }

    if (active->clients[pos].is_muted) {
        send_message("You have the right to remain silent!", speaker);
        pthread_mutex_unlock(&lock);
        return;
    }

    int max_len = Socket::buffer_size - name.size();
    vector<int> to_be_removed;

    for (Client client : active->clients) {
        int conn = client.conn;

        int receiver = active->find(conn);
        if (active->clients[receiver].channel_name != active->clients[pos].channel_name) continue;

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

// Thread para iteragir com o cliente
void* server_thread(void* arg) {    
    int failed = 0;
    bool quit = false;
    int new_client = *((int*)arg);
    int pos = active->find(new_client);
    string nome = active->clients[pos].name;
    spread_message("A new client has joined us!", new_client);

    while (!quit) {
        string message = socket_server->Read(new_client);
        cout << message << endl;
        socket_server->Check();

        if (message.empty()) {
            failed++;
            
            if (failed >= Socket::max_retries) message = "/quit";
            else continue;
        } else {
            failed = 0;
        }

        if (message == "/quit") {
            spread_message("Someone left the server", new_client);
            active->remove(new_client);
            quit = true;

        } else if (message == "/ping") {
            send_message("pong!", new_client);

        } else if(message.size() >= 10 && message.substr(0, 9) == "/nickname") {
            string new_nickname = message.substr(10, message.size());
            int pos = active->find(new_client);
            active->clients[pos].name = new_nickname;
        
        } else if (message.size() >= 6 && message.substr(0, 5) == "/join") {
            string channel_name = message.substr(6, message.size());
            
            int pos = active->find(new_client);
            bool is_admin = (channels[channel_name] == 0);
            
            if(channel_name[0] == '&' and !is_admin){
                auto permissions = channels_permissions[channel_name];
                auto can_join = find(permissions.begin(), permissions.end(), new_client) != permissions.end();
                if (!can_join) {
                    send_message("You're not welcome here >:(", new_client);
                    continue;
                }
            }

            active->clients[pos].channel_name = channel_name;
            active->clients[pos].is_admin = is_admin;    
            
            send_message("Joined channel " + channel_name, new_client);

            if(is_admin) {
                channels[channel_name] = 1;
                channels_permissions[channel_name] = {};
                channels_permissions[channel_name].push_back(new_client);
                send_message("You own this ****!", new_client);
            }
            
            channels[channel_name]++;

        } else if (message.size() >= 6 && message.substr(0, 5) == "/kick") {
            int pos_client = active->find(new_client);
            
            if (active->clients[pos_client].is_admin == false) {
                send_message("You can't be that bad!", new_client);
                continue;
            }
            string to_be_kicked = message.substr(6, message.size());
            int pos = active->find(active->clients[pos_client].channel_name, to_be_kicked);

            if (active->clients[pos].channel_name != active->clients[pos_client].channel_name) {
                send_message("This user is not here", new_client);
                continue;
            }

            send_message("Admin hates you", active->clients[pos].conn);
            active->clients[pos].channel_name = "$";
            active->clients[pos].is_admin = false;
            channels[active->clients[pos_client].channel_name]--;
            

        } else if (message.size() >= 6 && message.substr(0, 5) == "/mute") {
            int pos_client = active->find(new_client);
            
            if (active->clients[pos_client].is_admin == false) {
                send_message("The Force is not with you!!", new_client);
                continue;
            } 
            string to_be_kicked = message.substr(6, message.size());
            int pos = active->find(active->clients[pos_client].channel_name, to_be_kicked);

            if (pos == -1 or active->clients[pos].channel_name != active->clients[pos_client].channel_name) {
                send_message("This user is not here", new_client);
                continue;
            }

            send_message("Admin can't stand you anymore", active->clients[pos].conn);
            active->clients[pos].is_muted = true;
            
        } else if (message.size() >= 8 && message.substr(0, 7) == "/unmute") {
            int pos_client = active->find(new_client);
            
            if (active->clients[pos_client].is_admin == false) {
                send_message("The Force is not with you!!", new_client);
                continue;
            } 
            string to_be_kicked = message.substr(8, message.size());
            int pos = active->find(active->clients[pos_client].channel_name, to_be_kicked);

            if (pos == -1 or active->clients[pos].channel_name != active->clients[pos_client].channel_name) {
                send_message("This user is not here", new_client);
                continue;
            }

            send_message("You have your voice again, use it wisely", active->clients[pos].conn);
            active->clients[pos].is_muted = false;
            
               
        } else if (message.size() >= 7 && message.substr(0, 6) == "/whois") {
            int pos_client = active->find(new_client);
            
            if (active->clients[pos_client].is_admin == false) {
                send_message("The Force is not with you!!", new_client);
                continue;
            } 
            
            string to_be_found = message.substr(7, message.size());
            int pos = active->find(active->clients[pos_client].channel_name, to_be_found);
            
            if (pos == -1 or active->clients[pos].channel_name != active->clients[pos_client].channel_name) {
                send_message("This user is not here", new_client);
                continue;
            }

            send_message(active->clients[pos].name + " is " + active->clients[pos].ip, new_client);
            
        } else if (message.size() >= 8 && message.substr(0, 7) == "/invite") {
            int pos_client = active->find(new_client);
            
            if (active->clients[pos_client].is_admin == false) {
                send_message("The Force is not with you!!", new_client);
                continue;
            } 
            
            string to_be_found = message.substr(8, message.size());
            int pos = active->find(to_be_found);
            
            if (pos == -1) {
                send_message("This user doesn't exist", new_client);
                continue;
            }

            send_message("Admin wants to have the pleasure of your company at " + active->clients[pos_client].channel_name + ". To join this channel, type /join" + active->clients[pos_client].channel_name, active->clients[pos].conn);
            channels_permissions[active->clients[pos_client].channel_name].push_back(active->clients[pos].conn);
            // for(auto v: channels_permissions[active->clients[pos_client].channel_name])
            //     cout << v << " ";
            // cout << 
            
        } else {
            spread_message(message, new_client);
        }

        message = "";
    }

    
    cout << nome << " has left us :(" << endl;
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
        cout << "while\n";
        int new_client = socket_server->Accept();
        cout << "accepted\n";
        socket_server->Check();
        
        if (active->clients.size() == Socket::max_clients) {
            socket_server->Write("Too many people here, you are not welcome\n");
            continue;
        } 

        string nickname = socket_server->Read(new_client);
        string ip = socket_server->Read(new_client);

        cout << "New client: " << nickname << endl;
        active->insert(new_client, nickname, ip);
        cout << "Done" << endl;

        if (pthread_create(&tid, NULL, server_thread, &new_client) != 0)
            cout << "Failed to create thread" << endl;
    }

    return 0;
}