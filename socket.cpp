#include "socket.h"

Socket::Socket(int port = DEFAULT_PORT, string ip = LOCALHOST) {
	cout << "Creating socket\n";
	this->port = port;
    this->ip = ip;

    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sock_fd == -1) {
        this->error.set_occurred();
        this->error.set_message("Could not create socket T.T\n");
    }

    //zera o serv_addr
    bzero(&serv_addr, sizeof(&serv_addr));
}

Socket::Socket(int sock_fd, sockaddr_in serv_addr) {
	cout << "Creating socket\n";	
	this->sock_fd = sock_fd;
	this->serv_addr = serv_addr;
}

Socket::~Socket() {
	cout << "Deleting socket\n";
	Disconnect();
}

void Socket::Bind(){
	cout << "Binding socket\n";
	
	//IPV4
	this->serv_addr.sin_family = AF_INET;
	//Endereco do host em network byte order
	this->serv_addr.sin_addr.s_addr = inet_addr((this->ip).c_str());
	//Port em network byte order
	this->serv_addr.sin_port = htons(this->port);

	int status = bind(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (status != 0){
		this->error.set_occurred();
		this->error.set_message("Could not bind with the server ;-;\n");
		return;
	}
}

void Socket::Listen() {
	cout << "Listining\n";

	listen(this->sock_fd, 2);
}

void Socket::Connect() {
	cout << "Connecting socket\n";
	
	//IPV4
	this->serv_addr.sin_family = AF_INET;
	//Endereco do host em network byte order
	this->serv_addr.sin_addr.s_addr = inet_addr((this->ip).c_str());
	//Port em network byte order
	this->serv_addr.sin_port = htons(this->port);

	int status = connect(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (status != 0) {
		this->error.set_occurred();
		this->error.set_message("Could not connect with the server =(\n");
		return;
	}
    
    this->connected = true;
}

Socket* Socket::Accept() {
	cout << "Accepting\n";
	
	sockaddr_in received_addr;
	socklen_t addr_size = sizeof(received_addr);

	int connection_fd = accept(this->sock_fd, (struct sockaddr *)&received_addr, &addr_size);

	if (connection_fd < 0) {
		this->error.set_occurred();
		this->error.set_message("Server didn't accept, sorry =(\n");
		return new Socket();
	}

	return new Socket(connection_fd, received_addr);
}

void Socket::Disconnect() {
	int status = close(this->sock_fd);

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not disconnect... =(\n");
	}
}

string Socket::Read() {
	cout << "Reading\n";

	char helper[buffer_size];
	
	bzero(helper, sizeof(helper));	
	read(this->sock_fd, helper, sizeof(helper));
	string ret(helper);
	return ret;
}

void Socket::Write(string msg) {
	cout << "Writing\n";

	char helper[buffer_size];
	
	bzero(helper, sizeof(helper));
	strcpy(helper, msg.c_str());
	write(this->sock_fd, helper, sizeof(helper));
}

string Socket::Get_error(){
	return this->error.get_message();
}

bool Socket::Check_error() {
	return this->error.has_occurred();
}