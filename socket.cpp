#include "socket.h"

Socket::Socket(string ip, string name, int port) {
	if (log) cout << "Creating socket o/\n";
	
	this->name = name;
	this->port = port;
	this->ip = ip;

    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	this->conn_fd = this->sock_fd;

	if (this->sock_fd == -1) {
        this->error.set_occurred();
        this->error.set_message("Could not create socket T.T\n");
    }

    //zera o serv_addr
    bzero(&serv_addr, sizeof(&serv_addr));
}

Socket::~Socket() {
	if (log) cout << "Deleting socket =(\n";
	Disconnect();
}

void Socket::Bind(){
	if (log) cout << "Binding socket =)\n";
	
	//IPV4
	this->serv_addr.sin_family = AF_INET;
	//Endereco do host em network byte order
	this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr((this->ip).c_str());
	//Port em network byte order
	this->serv_addr.sin_port = htons(this->port);

	int status = bind(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (status == -1){
		this->error.set_occurred();
		this->error.set_message("Could not bind with the server ;-;\n");
		return;
	}
}

void Socket::Listen() {
	if (log) cout << "Listening 0.0\n";

	listen(this->sock_fd, 2);
}

void Socket::Connect() {
	if (log) cout << "Connecting socket XD\n";
	
	//IPV4
	this->serv_addr.sin_family = AF_INET;
	//Endereco do host em network byte order
	this->serv_addr.sin_addr.s_addr = inet_addr((this->ip).c_str());
	//Port em network byte order
	this->serv_addr.sin_port = htons(this->port);

	int status = connect(this->sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not connect with the server =(\n");
		return;
	}
    
    this->connected = true;
}

void Socket::Accept() {
	if (log) cout << "Accepting =P\n";
	
	sockaddr_in received_addr;
	socklen_t addr_size = sizeof(received_addr);

	int connection_fd = accept(this->sock_fd, (struct sockaddr *)&received_addr, &addr_size);

	if (connection_fd == -1) {
		this->error.set_occurred();
		this->error.set_message("Server didn't accept, sorry =(\n");
		return;
	}
	
	this->conn_fd = connection_fd;
}

void Socket::Disconnect() {
	if (log) cout << "Disconnecting =(\n";

	int status = close(this->sock_fd);

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not disconnect... =(\n");
	}
}

string Socket::Read() {
	if (log) cout << "Reading =@\n";

	char helper[buffer_size + 1];
	bzero(helper, sizeof(helper));	
	
	int status =  read(this->conn_fd, helper, sizeof(helper));

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not read... =(\n");
	}
	
	string ret(helper);
	return ret;
}

void Socket::Write(string msg) {
	if (log) cout << "Writing =S\n";

	char helper[buffer_size + 1];
	bzero(helper, sizeof(helper));
	strcpy(helper, msg.c_str());

	int status = write(this->conn_fd, helper, sizeof(helper));

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not write... =(\n");
	}
}

string Socket::Get_error(){
	return this->error.get_message();
}

bool Socket::Check_error() {
	return this->error.has_occurred();
}

void Socket::Set_not_error() {
	this->error.set_not_occurred();
}