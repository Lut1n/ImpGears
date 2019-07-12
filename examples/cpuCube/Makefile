DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS= -lsfml-graphics -lsfml-window -lsfml-system -limpgears-core -limpgears-graphics -limpgears-geometry -limpgears-scenegraph  -limpgears-renderer -limpgears-descriptors -limpgears-plugins

all: viewer

viewer: viewer.o
	$(CXX) -o viewer viewer.o -L /usr/lib $(LDFLAGS)

viewer.o: viewer.cpp
	$(CXX) $(CXXFLAGS) -I /usr/include/ImpGears -c viewer.cpp -o viewer.o
	
.PHONY : clean

clean:
	rm -rf viewer.o viewer
