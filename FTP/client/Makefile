CC = g++
FLAGS = -std=c++11
COMPILE_FLAGS = $(FLAGS) -c
all: client

client: main.o client.o json.o User.o ../Utils/utils.o
	$(CC) main.o client.o json.o User.o ../Utils/utils.o $(FLAGS) -o client

main.o: main.cpp client.h
	$(CC) $(COMPILE_FLAGS) main.cpp

client.o: client.cpp client.h ../Json/json.h ../Utils/utils.cpp ../Utils/utils.h
	$(CC) $(COMPILE_FLAGS) client.cpp

json.o: ../Json/json.cpp ../Json/json.h ../server/User.h
	$(CC) $(COMPILE_FLAGS) ../Json/json.cpp

User.o: ../server/User.cpp ../server/User.h
	$(CC) $(COMPILE_FLAGS) ../server/User.cpp

../Utils/utils.o: ../Utils/utils.cpp ../Utils/utils.h
	g++ $(COMPILE_FLAGS) ../Utils/utils.cpp -o ../Utils/utils.o

.PHONY: clean

clean:
				rm *.o
				rm client