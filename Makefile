SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.hpp)
FLAGS=-DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpic++ $(SOURCES) $(FLAGS) -o main -lpthread

clear: clean

clean:
	rm main

run: main
	mpirun -np 5 --oversubscribe ./main
