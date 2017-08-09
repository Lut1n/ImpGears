DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system libimpgears.so

all: nodal

nodal: nodal.o
	$(CXX) -g -o nodal nodal.o $(LDFLAGS)

nodal.o: nodal.cpp
	$(CXX) $(CXXFLAGS) -g -I ../../include/ImpGears -c nodal.cpp -o nodal.o

.PHONY : clean

clean:
	rm -rf nodal.o nodal
