COMPILE_FLAGS = -std=c++11 -c

all: server

server: main.o server.o ../Json/json.o User.o ../Utils/utils.o
	g++ main.o server.o ../Json/json.o User.o ../Utils/utils.o $(FLAGS) -o server -lpthread

main.o: main.cpp server.h
	g++ $(COMPILE_FLAGS) main.cpp

server.o: server.cpp server.h ../Json/json.h ../Utils/utils.h
	g++ $(COMPILE_FLAGS) server.cpp

../Json/json.o: ../Json/json.cpp ../Json/json.h User.h
	g++ $(COMPILE_FLAGS) ../Json/json.cpp -o ../Json/json.o

User.o: User.cpp User.h
	g++ $(COMPILE_FLAGS) User.cpp

../Utils/utils.o: ../Utils/utils.cpp ../Utils/utils.h
	g++ $(COMPILE_FLAGS) ../Utils/utils.cpp -o ../Utils/utils.o

.PHONY: clean

clean:
		rm ../Utils/*.o
		rm ../Json/*.o
		rm *.o
		rm server
		rm log.txt