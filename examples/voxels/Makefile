DEBUG=no
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system libimpgears.so

all: voxels

voxels: voxels.o SFMLContextInterface.o
	$(CXX) -o voxels voxels.o SFMLContextInterface.o $(LDFLAGS)

SFMLContextInterface.o: SFMLContextInterface.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c SFMLContextInterface.cpp -o SFMLContextInterface.o
	
voxels.o: voxels.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c voxels.cpp -o voxels.o

.PHONY : clean

clean:
	rm -rf voxels.o voxels
