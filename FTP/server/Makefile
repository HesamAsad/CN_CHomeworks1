COMPILE_FLAGS = -std=c++11 -c

all: server

server: main.o server.o json.o User.o utils.o
	g++ main.o server.o json.o User.o utils.o $(FLAGS) -o server -lpthread

main.o: main.cpp server.h
	g++ $(COMPILE_FLAGS) main.cpp

server.o: server.cpp server.h ../Json/json.h ../Utils/utils.h
	g++ $(COMPILE_FLAGS) server.cpp

json.o: ../Json/json.cpp ../Json/json.h User.h
	g++ $(COMPILE_FLAGS) ../Json/json.cpp

User.o: User.cpp User.h
	g++ $(COMPILE_FLAGS) User.cpp

utils.o: ../Utils/utils.cpp ../Utils/utils.h
	g++ $(COMPILE_FLAGS) ../Utils/utils.cpp

.PHONY: clean

clean:
		rm *.o
		rm server
		rm log.txt