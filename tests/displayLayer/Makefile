DEBUG=yes
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

test.o: viewer.cpp
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c test.cpp -o test.o

.PHONY : clean

clean:
	rm -rf test.o test
