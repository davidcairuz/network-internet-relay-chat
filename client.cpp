#include "socket.h"
#include <algorithm>
#include <pthread.h>

string nickname = "";
bool quit = false;
Socket* client_socket;

void Sigint_handler(int sig_num) {
    signal(SIGINT, Sigint_handler);
    cout << "\nCannot quit using crtl+c\n";
    fflush(stdout);
}

string get_menu() {
    return "-------- Options -------- \n /connect: Connects to server \n /quit: Quits the connection \n /ping: Pings server\n /menu: Displays menu\n";
}

void* client_receive_thread(void* arg) {
    string message = "";

    while (!quit) {
        do {
            message = client_socket->Read(client_socket->Get_conn_fd());
            client_socket->Check();
            cout << message << "\n";
            
            if (message == "Too many people here, you are not welcome\n") {
                quit = true;
                break;
            } else if (message.empty()) {
                cout << "Server is down" << endl;
                quit = true;
                break;
            }

            message = "";

        } while (message.size() == Socket::buffer_size && !quit);
    }

    delete client_socket;
    pthread_exit(NULL);
}

void* client_send_thread(void* arg) {
    cout << "Connected to server" << endl;
    client_socket->Write(nickname);
    string message = "";

    while (!quit) {
        if (!getline(cin, message, '\n')) {
            cout << "Tiau" << endl;
            message = "/quit";
        }

        if (message == "/menu") {
            cout << get_menu();
            continue;
        }

        for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
            client_socket->Write(message.substr(i, Socket::buffer_size));
            client_socket->Check();
        }

        if (message == "/quit") {
            quit = true;
        }
    }

    delete client_socket;
    pthread_exit(NULL);
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

int main(int argc, char* argv[]) {
    signal(SIGINT, Sigint_handler);

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

        if (command == "/menu") cout << get_menu();
        else if(command == "/quit") exit(0);
    }

    client_socket = new Socket(LOCALHOST, nickname);
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

    while (!quit);

    cout << "É nois flw vlw" << endl;    
    return 0;
}