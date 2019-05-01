DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS= ../../build/libimpgears.so

all: test

test: test.o
	$(CXX) -o test test.o -L /usr/lib $(LDFLAGS)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -I /usr/include -c test.cpp -o test.o

.PHONY : clean

clean:
	rm -rf test.o test
