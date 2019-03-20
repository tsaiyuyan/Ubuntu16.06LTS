CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall -pedantic
BIN=helloworld.out
LDLIBS=-lm # list libs here
SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)


all: $(OBJ)
	$(CXX) -o $(BIN) $^
%.o: %.c
	$(CXX) %@ -c $<

.PHONY:clean
clean: 
	rm -rf *.o
	rm $(BIN)
