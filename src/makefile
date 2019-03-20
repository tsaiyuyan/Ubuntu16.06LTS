CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall -pedantic
BIN=test_pthread.out
LDFLAGS=-pthread # list libs here
SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)


all: $(OBJ)
	$(CXX) -o $(BIN) $^ $(LDFLAGS)
%.o: %.c
	$(CXX) %@ -c $<

.PHONY:clean
clean: 
	rm -rf *.o
	rm $(BIN)
