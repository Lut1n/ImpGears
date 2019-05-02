DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++17 -g
else
	CXXFLAGS=-Wall -std=c++17
endif

LDFLAGS= -lstdc++fs -lsfml-graphics -lsfml-window -lsfml-system ../../build/libimpgears.so

all: viewer

viewer: viewer.o geoOp.o
	$(CXX) -o viewer viewer.o geoOp.o -L /usr/lib $(LDFLAGS)

viewer.o: viewer.cpp JsonGeometryOp.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c viewer.cpp -o viewer.o

geoOp.o:  JsonGeometryOp.cpp JsonGeometryOp.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c  JsonGeometryOp.cpp -o geoOp.o

.PHONY : clean

clean:
	rm -rf viewer.o geoOp.o viewer
