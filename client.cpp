#include "socket.h"
#include <algorithm>
#include <pthread.h>

string nickname = "";
string channel = "";
bool quit = false;
Socket* client_socket;

// Lida com Control+C
void Sigint_handler(int sig_num) {
    signal(SIGINT, Sigint_handler);
    cout << "\nCannot quit using crtl+c\n";
    fflush(stdout);
}

bool check_channel_name(string name){
    if(name.size() == 0 or name.size() > 200) return false;
    if(name[0] != '#' and name[0] != '&') return false;
    if(name.find(' ') != name.npos) return false;
    if(name.find(',') != name.npos) return false;
    if(name.find((char)7) != name.npos) return false;
    return true;
}

string get_menu() {
    return "-------- Options -------- \n /connect: Connects to server \n /join: Joins a channel \n /quit: Quits the connection \n /ping: Pings server\n /menu: Displays menu\n /nickname new_nickname: Changes nickname to the new_nickname\n /kick: Kicks a client if\n /mute: Mutes a client\n /unmute: Unmutes a client\n /invite: Invites a client to your channel\n";
}

bool check_letter(char letter) {
    return (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') || (letter  == '.') || (letter == '_') || letter == '-';
}

bool check_username(string username) {
    if (username.size() <= 2) return false;
    if (username == "server") return false;

    for (char letter : username) {
        if (!check_letter(letter)) return false;
    }
    return true;
}

string get_nickname() {
    string nickname = "";
    cout << "Type your nickname (a-z, A-Z, ., _, -, size > 2): ";
    
    if(!getline(cin, nickname)) {
        cout << "Quitting" << endl;
        exit(0);
    }
    
    while (!check_username(nickname)) {
        cout << "Type a valid username (a-z, A-Z, ., _, -, size > 2): ";
        if(!getline(cin, nickname)) {
           cout << "Quitting" << endl;
           exit(0);
        }
    }

    return nickname;
}

// Thread para receber mensagens do servidor
void* client_receive_thread(void* arg) {
    string message = "";
    int failed = 0;

    while (!quit) {
        do {
            message = client_socket->Read(client_socket->Get_conn_fd());
            client_socket->Check();
            cout << message << "\n";
            
            if (message == "Too many people here, you are not welcome\n") {
                quit = true;
                break;
            } else if (message.empty()) {
                failed++;
                sleep(1);
                
                if (failed >= Socket::max_retries) {
                    cout << "Server is down" << endl;
                    break;
                } else 
                    continue;
            
            } else {
                failed = 0;
            }

            message = "";

        } while (message.size() == Socket::buffer_size && !quit);
    }

    pthread_exit(NULL);
}

// Thread para enviar mensagens para o servidor
void* client_send_thread(void* arg) {
    cout << "Connected to server" << endl;
    client_socket->Write(nickname);
    client_socket->Write(client_socket->Get_ip());
    string message = "";

    while (!quit) {
        if (!getline(cin, message, '\n')) {
            cout << "Tiau" << endl;
            message = "/quit";
        }

        if (message == "/menu") {
            cout << get_menu();
            continue;
            
        } else if(message.size() >= 10 && message.substr(0, 9) == "/nickname") {
            if (message.size () < 12) 
                cout << "Type a valid username (a-z, A-Z, ., _, -, size > 2)";
            
            string new_nickname = message.substr(10, message.size());
            
            if (check_username(nickname)) {
                nickname = new_nickname;
                cout << "Nickname updated to: " << nickname << endl;        
            }

        } else if(message.size() >= 6 && message.substr(0, 5) == "/join") {
            string new_channel = message.substr(6, message.size());
            
            if (check_channel_name(new_channel)) {
                channel = new_channel;
            } else {
                cout << "Invalid channel " << channel << endl;
                continue;
            }
        }

        for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
            client_socket->Write(message.substr(i, Socket::buffer_size));
            client_socket->Check();
        }

        if (message == "/quit") {
            quit = true;
        }

        message = "/quit";
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, Sigint_handler);

    if (argc < 1) {
        cout << "Provide your ip, dumbass\n";
        return 1; 
    }

    // checar ip
    string ip = argv[1];

    pthread_t tid_receive;
    pthread_t tid_send;
    string command = "";
    
    nickname = get_nickname();
    cout << get_menu();

    while (command != "/connect") {
        if(!getline(cin, command, '\n')) {
            cout << "quitting" << endl;
            exit(0);
        }

        if (command == "/menu") 
            cout << get_menu();
        else if(command == "/quit") 
            exit(0);
        else if(command == "/ping") 
            cout << "You've got to be connected!" << endl;
        else if(command.size() >= 9 && command.substr(0, 9) == "/nickname") {
            string new_nickname = command.substr(10, command.size());
            
            if (check_username(nickname)) {
                nickname = new_nickname;
                cout << "Nickname updated to: " << nickname << endl;        
            } else {
                cout << "Type a valid username (a-z, A-Z, ., _, -, size > 2)";
            }    
        } else if(command.size() >= 5 && command.substr(0, 5) == "/join") {
            cout << "You've got to be connected!" << endl;
        } else if (command.size() >= 5 && command.substr(0, 5) == "/kick") {
            cout << "You've got to be connected!" << endl;
        } else if (command.size() >= 5 && command.substr(0, 5) == "/mute") {
            cout << "You've got to be connected!" << endl;
        } else if (command.size() >= 7 && command.substr(0, 7) == "/unmute") {
            cout << "You've got to be connected!" << endl;
        }
    }

    client_socket = new Socket(ip, nickname);
    client_socket->Check();

    client_socket->Connect();
    client_socket->Check();

    if (pthread_create(&tid_send, NULL, client_send_thread, NULL) != 0) {
        cout << "Failed to create thread to send message" << endl;
        return 1;
    }
    if (pthread_create(&tid_receive, NULL, client_receive_thread, NULL) != 0) {
        cout << "Failed to create thread to receive message" << endl;
        return 1;
    }

    while (!quit); //Checa aqui sem problemas

    cout << "É nois flw vlw" << endl;
    return 0;
}