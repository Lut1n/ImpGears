DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++17 -g
else
	CXXFLAGS=-Wall -std=c++17
endif

LDFLAGS= -lstdc++fs -lsfml-graphics -lsfml-window -lsfml-system -limpgears-core -limpgears-geometry -limpgears-graphics -limpgears-scenegraph -limpgears-descriptors -limpgears-renderer

all: viewer

viewer: viewer.o
	$(CXX) -o viewer viewer.o -L /usr/lib $(LDFLAGS)

viewer.o: viewer.cpp
	$(CXX) $(CXXFLAGS) -I /usr/local/include -c viewer.cpp -o viewer.o

.PHONY : clean

clean:
	rm -rf viewer.o viewer
