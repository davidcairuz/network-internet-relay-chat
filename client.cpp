#include "socket.h"
#include <algorithm>
#include <pthread.h>

// void Sigint_handler(int sig_num) {
//     signal(SIGINT, Sigint_handler);
//     cout << "\nCannot quit using crtl+c\n";
//     fflush(stdout); 
// }

string nickname = "";
bool quit = false;
Socket* client_socket;

void* client_receive_thread(void* arg) {
    string message = "";

    while (!quit) {
        do {
            message = client_socket->Read(client_socket->Get_conn_fd());
            client_socket->Check();
            cout << message << "\n";
        } while (message.size() == Socket::buffer_size);
    }

    delete client_socket;
    pthread_exit(NULL);
}

void* client_send_thread(void* arg) {
    cout << "Connected to server" << endl;
    string message = "";

    while (!quit) {
        getline(cin, message, '\n');

        if (message == "/quit") {
            quit = true;
            break;
        }

        for (int i = 0; (unsigned int)i < message.size(); i += Socket::buffer_size) {
            client_socket->Write(message.substr(i, Socket::buffer_size));
            client_socket->Check();
        }
    }

    delete client_socket;
    pthread_exit(NULL);
}

string get_nickname() {
    string nickname = "";
    cout << "Type your nickname: ";
    getline(cin, nickname);
    return nickname;
}

int main(int argc, char* argv[]) {
    // signal(SIGINT, Sigint_handler); 

    pthread_t tid;
    string command = "";
    nickname = get_nickname();

    while (command != "/connect") getline(cin, command);

    client_socket = new Socket(LOCALHOST, nickname);
    client_socket->Check();

    client_socket->Connect();
    client_socket->Check();

    if (pthread_create(&tid, NULL, client_send_thread, NULL) != 0) {
        cout << "Failed to create thread to send message" << endl;
        return 1;
    }

    if (pthread_create(&tid, NULL, client_receive_thread, NULL) != 0) {
        cout << "Failed to create thread to receive message" << endl;
        return 1;
    }

    while (!quit);

    cout << "É nois flw vlw" << endl;    
    return 0;
}