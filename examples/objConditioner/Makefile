DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-limpgears

all: objConditioner

objConditioner: objConditioner.o
	$(CXX) $(LDFLAGS) objConditioner.o -o objConditioner

objConditioner.o: objConditioner.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c objConditioner.cpp -o objConditioner.o

.PHONY : clean

clean:
	rm -rf objConditioner.o objConditioner
