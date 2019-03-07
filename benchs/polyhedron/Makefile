DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system ../../libimpgears.so

all: viewer

viewer: viewer.o equation.o Triangulator.o
	$(CXX) -o viewer viewer.o equation.o Triangulator.o $(LDFLAGS)

viewer.o: viewer.cpp initGraph.cpp equation.h shader.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c viewer.cpp -o viewer.o

equation.o: equation.cpp equation.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c equation.cpp -o equation.o
	
Triangulator.o : Triangulator.cpp Triangulator.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c Triangulator.cpp -o Triangulator.o

.PHONY : clean

clean:
	rm -rf viewer.o equation.o Triangulator.o viewer
