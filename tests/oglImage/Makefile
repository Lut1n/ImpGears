DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++17 -g
else
	CXXFLAGS=-Wall -std=c++17
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -limpgears-core -limpgears-geometry -limpgears-graphics -limpgears-scenegraph -limpgears-renderer -limpgears-descriptors -limpgears-plugins

all: test

test: test.o
	$(CXX) -o test test.o -L /usr/lib $(LDFLAGS)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -I /usr/include/ImpGears -c test.cpp -o test.o

.PHONY : clean

clean:
	rm -rf test.o test
