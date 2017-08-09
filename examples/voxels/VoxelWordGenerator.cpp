#include <ctime>
#include <stdlib.h>

#include <Core/Perlin.h>

#include "VoxelWordGenerator.h"

IMPGEARS_BEGIN

VoxelWordGenerator* VoxelWordGenerator::instance = IMP_NULL;

//--------------------------------------------------------------
VoxelWordGenerator::VoxelWordGenerator()
{
    InitRand();
    VoxelWordGenerator::instance = this;
}

//--------------------------------------------------------------
VoxelWordGenerator::~VoxelWordGenerator()
{
    VoxelWordGenerator::instance = IMP_NULL;
}

//--------------------------------------------------------------
VoxelWordGenerator* VoxelWordGenerator::GetInstance()
{
    if(VoxelWordGenerator::instance == IMP_NULL)
        new VoxelWordGenerator();

    return VoxelWordGenerator::instance;
}

//--------------------------------------------------------------
void VoxelWordGenerator::Generate(VoxelWorld* _world)
{
    for(imp::Uint32 x = 0; x<_world->GetSizeX(); ++x)
    for(imp::Uint32 y = 0; y<_world->GetSizeY(); ++y)
    {
        imp::Int32 localElevation = ((perlinOctave((double)x/_world->GetSizeX(), (double)y/_world->GetSizeY(), 0.0, 6, 0.7, 4.0)+1.0)/2.0)*(_world->GetSizeZ());
		
        imp::Int32 z = 0;
        for(; z<localElevation-1; ++z)
		{
			double caveValue = perlinOctave((double)x/_world->GetSizeX(),(double)y/_world->GetSizeY(),(double)z/_world->GetSizeZ(), 6, 0.3, 7.0);
			if(caveValue < 0.0)
				_world->SetValue(x,y,(imp::Uint32)z, 0);
			else 
            	_world->SetValue(x,y,(imp::Uint32)z, 4);
		}

		for(;z<localElevation; ++z)
			_world->SetValue(x,y,(imp::Uint32)z,1);

		imp::Int32 height = _world->GetSizeZ();
        for(; z<height; ++z)
            _world->SetValue(x,y,(imp::Uint32)z,0);
    }

    fprintf(stdout, "generate termine\n");
}

//--------------------------------------------------------------
void VoxelWordGenerator::Generate(VoxelWorld* _world, const imp::ImageData* _heightmap)
{
	    imp::Uint32 height = _world->GetSizeZ();

	    int imgw = _heightmap->getWidth();
	    int imgh = _heightmap->getHeight();

		for(imp::Uint32 x = 0; x<_world->GetSizeX(); ++x)
		for(imp::Uint32 y = 0; y<_world->GetSizeY(); ++y)
		{
			int imgx = x*imgw/_world->GetSizeX();
			int imgy = y*imgh/_world->GetSizeY();

            imp::Pixel pixel = _heightmap->getPixel(imgx, imgy);
			int localElevation = (pixel.red + pixel.green + pixel.blue)/3;

			localElevation = localElevation*(unsigned int)height/256;

			int z = 0;
			for(; z<localElevation-3; ++z)
				_world->SetValue(x,y,z, 4);

			for(;z<localElevation; ++z)
				_world->SetValue(x,y,z,1);

			for(; z<(int)height; ++z)
				_world->SetValue(x,y,z,0);
		}
}

//--------------------------------------------------------------
void VoxelWordGenerator::InitRand()
{
    srand(time(NULL));
}

//--------------------------------------------------------------
imp::Uint32 VoxelWordGenerator::Rand(imp::Uint32 _min, imp::Uint32 _max)
{
    return (rand()%(_max - _min) + _min);
}

IMPGEARS_END
