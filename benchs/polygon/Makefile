DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS= -lsfml-graphics -lsfml-window -lsfml-system ../../libimpgears.so

all: viewer

viewer: viewer.o ComplexPolygon.o SimplePolygon.o Path.o
	$(CXX) -o viewer viewer.o ComplexPolygon.o SimplePolygon.o Path.o -L /usr/lib $(LDFLAGS)


viewer.o: viewer.cpp ComplexPolygon.h SimplePolygon.h Path.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c viewer.cpp -o viewer.o

ComplexPolygon.o: ComplexPolygon.cpp ComplexPolygon.h SimplePolygon.h Path.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c ComplexPolygon.cpp -o ComplexPolygon.o

SimplePolygon.o: SimplePolygon.cpp SimplePolygon.h Path.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c SimplePolygon.cpp -o SimplePolygon.o

Path.o: Path.cpp Path.h
	$(CXX) $(CXXFLAGS) -I ../../include/ImpGears -c Path.cpp -o Path.o
	
.PHONY : clean

clean:
	rm -rf viewer.o ComplexPolygon.o SimplePolygon.o Path.o viewer
