DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS= -limpgears-core -limpgears-graphics -limpgears-geometry -limpgears-descriptors

all: test

test: test.o
	$(CXX) -o test test.o -L /usr/lib $(LDFLAGS)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -I /usr/include/ImpGears -c test.cpp -o test.o

.PHONY : clean

clean:
	rm -rf test.o test
