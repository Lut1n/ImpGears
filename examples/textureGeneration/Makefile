DEBUG=no
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system libimpgears.so

all: test

test: test.o
	$(CXX) -o test test.o $(LDFLAGS)

test.o: main.cpp
	$(CXX) -o test.o $(CXXFLAGS) -I ../../include/ImpGears -c main.cpp

.PHONY : clean

clean:
	rm -rf test.o test
