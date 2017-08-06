DEBUG=no
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -std=c++11 -g
else
	CXXFLAGS=-Wall -std=c++11
endif

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system libimpgears.so

all: voxels

voxels: voxels.o ChunkData.o VBOChunk.o VoxelWordGenerator.o VoxelWorld.o
	$(CXX) -o voxels voxels.o ChunkData.o VBOChunk.o VoxelWordGenerator.o VoxelWorld.o $(LDFLAGS)

voxels.o: voxels.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c voxels.cpp -o voxels.o

ChunkData.o: ChunkData.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c ChunkData.cpp -o ChunkData.o

VBOChunk.o: VBOChunk.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c VBOChunk.cpp -o VBOChunk.o

VoxelWordGenerator.o: VoxelWordGenerator.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c VoxelWordGenerator.cpp -o VoxelWordGenerator.o

VoxelWorld.o: VoxelWorld.cpp
	$(CXX) $(CXXFLAGS) -I ../../includes -c VoxelWorld.cpp -o VoxelWorld.o

.PHONY : clean

clean:
	rm -rf *.o voxels
