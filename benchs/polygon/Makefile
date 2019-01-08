DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS= -lsfml-graphics -lsfml-window -lsfml-system ../../libimpgears.so

all: viewer

viewer: viewer.o PolygonSet.o Cycle.o
	$(CXX) -o viewer viewer.o PolygonSet.o Cycle.o -L /usr/lib $(LDFLAGS)

viewer.o: viewer.cpp
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c viewer.cpp -o viewer.o

Cycle.o: Cycle.cpp
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c Cycle.cpp -o Cycle.o

PolygonSet.o: PolygonSet.cpp
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c PolygonSet.cpp -o PolygonSet.o
	
.PHONY : clean

clean:
	rm -rf viewer.o PolygonSet.o Cycle.o viewer
