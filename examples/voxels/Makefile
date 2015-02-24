DEBUG=no
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-limpgears

all: voxels

voxels: voxels.o
	$(CXX) $(LDFLAGS) voxels.o -o voxels

voxels.o: voxels.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c voxels.cpp -o voxels.o

.PHONY : clean

clean:
	rm -rf voxels.o voxels
