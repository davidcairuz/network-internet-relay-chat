all:
	g++ error.cpp socket.cpp client.cpp -o client -Wall
	g++ error.cpp socket.cpp server.cpp -o server -Wall

client:
	g++ error.cpp socket.cpp client.cpp -o client -Wall
	./client

server:
	g++ error.cpp socket.cpp server.cpp -o server -Wall
	./server

socket:
	g++ error.cpp socket.cpp -o socket -Wall

error:
	g++ error.cpp -Wall

clean: 
	rm -f *.o client server