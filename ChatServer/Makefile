FLAGS = -std=c++11

all: all_client all_server

all_client: client/client.o
	g++ -pthread $(FLAGS) client/client.o -o client/client

all_server: server/server.o server/main.o server/chatRoom.o server/client.o
	g++ $(FLAGS) server/server.o server/main.o server/chatRoom.o server/client.o -o server/server

server/main.o: server/main.cpp server/server.cpp server/server.h message.h
	g++ $(FLAGS) -c server/main.cpp -o server/main.o

server/server.o: server/server.cpp server/server.h message.h
	g++ $(FLAGS) -c server/server.cpp -o server/server.o

server/chatRoom.o: server/user.h server/chatRoom.h server/chatRoom.cpp
	g++ $(FLAGS) -c server/chatRoom.cpp -o server/chatRoom.o

server/client.o: server/client.h server/client.cpp server/chatRoom.h server/chatRoom.cpp
	g++ $(FLAGS) -c server/client.cpp -o server/client.o

client/client.o: client/client.cpp message.h
	g++ $(FLAGS) -c client/client.cpp -o client/client.o

clean:
	rm -rf server/*.o client/*.o server/server client/client
