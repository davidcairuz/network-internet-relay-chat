#include "socket.h"

Socket::Socket(string ip, string name, int port) {
	if (log) cout << "Creating socket\n";
	
	this->connected = false;
	this->name = name;
	this->port = port;
	this->ip = ip;

    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	this->conn_fd = this->sock_fd;

	if (this->sock_fd == -1) {
        this->error.set_occurred();
        this->error.set_message("Could not create socket T.T\n");
    }

	size_t size = sizeof(&serv_addr);
    bzero(&serv_addr, size);
}

Socket::~Socket() {
	Disconnect();
	if (log) cout << "Deleting socket(\n";
}

void Socket::Bind(){
	if (log) cout << "Binding socket\n";
	
	/* IPV4 */
	this->serv_addr.sin_family = AF_INET;

	/* Manda o socket ouvir em todas as interfaces disponíveis */
	this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* Port em network byte order */
	this->serv_addr.sin_port = htons(this->port);

	int status = bind(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	if (status == -1){
		this->error.set_occurred();
		this->error.set_message("Could not bind with the server ;-;\n");
		return;
	}
}

void Socket::Listen() {
	if (log) cout << "Listening\n";

	listen(this->sock_fd, max_clients);
}

void Socket::Connect() {
	if (log) cout << "Connecting socket\n";
	
	/* IPV4 */
	this->serv_addr.sin_family = AF_INET;
	
	/* Diz em que porta está o servidor a ser conectado */
	this->serv_addr.sin_addr.s_addr = inet_addr((this->ip).c_str());
	
	/* Port em network byte order */
	this->serv_addr.sin_port = htons(this->port);

	int status = connect(this->sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not connect with the server =(\n");
		return;
	}
    
    this->connected = true;
}

int Socket::Accept() {
	if (log) cout << "Accepting\n";
	
	sockaddr_in received_addr;
	socklen_t addr_size = sizeof(received_addr);

	int connection_fd = accept(this->sock_fd, (struct sockaddr *)&received_addr, &addr_size);

	if (connection_fd == -1) {
		this->error.set_occurred();
		this->error.set_message("Server didn't accept, sorry =(\n");
		return -1;
	}
	
	this->conn_fd = connection_fd;
	return connection_fd;
}

void Socket::Disconnect() {
	if (log) cout << "Disconnecting\n";

	int status = close(this->sock_fd);

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could no´t disconnect... =(\n");
	}
}

string Socket::Read(int conn_fd) {
	if (log) cout << "Reading\n";

	char helper[buffer_size + 1];
	bzero(helper, sizeof(helper));
	
	int status = read(conn_fd, helper, sizeof(helper));

	if (status == -1) {
		this->error.set_occurred();
		this->error.set_message("Could not read... =(\n");
	}
	
	string ret(helper);
	return ret;
}

int Socket::Write(string msg, int conn_fd, string id) {
	if (log) cout << "Writing\n";

	// Default conn_fd
	if (conn_fd == -1) conn_fd = this->conn_fd;

	char helper[buffer_size + 1];
	bzero(helper, sizeof(helper));
	strcpy(helper, msg.c_str());

	int status = write(conn_fd, helper, sizeof(helper));

	if (status < 1) {
		this->error.set_occurred();
		this->error.set_message("Could not write... =(\n");
	}

	return status;
}

void Socket::Update_nickname(string nickname) {
	this->name = nickname;
}

int Socket::Get_conn_fd() {
	return this->conn_fd;
}

void Socket::Check() {
    if (!this->Has_error()) return;
	
	cout << this->Get_error();
	delete this;
	exit(1);
}

string Socket::Get_error(){
	return this->error.get_message();
}

bool Socket::Has_error() {
	return this->error.has_occurred();
}