all:
	g++ error.cpp socket.cpp client.cpp -o client -Wall -pthread -lpthread -Wl,--no-as-needed
	g++ error.cpp socket.cpp server.cpp -o server -Wall -pthread -lpthread -Wl,--no-as-needed

client:
	g++ error.cpp socket.cpp client.cpp -o client -Wall -pthread -lpthread -Wl,--no-as-needed

server:
	g++ error.cpp socket.cpp server.cpp -o server -Wall -pthread -lpthread -Wl,--no-as-needed

socket:
	g++ error.cpp socket.cpp -o socket -Wall

error:
	g++ error.cpp -Wall

clean: 
	rm -f *.o client server
