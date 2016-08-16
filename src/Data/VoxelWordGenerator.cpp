#include <ctime>
#include <stdlib.h>

#include "Data/VoxelWordGenerator.h"
#include "Graphics/Perlin.h"

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
	Perlin::Config pConfig;
	pConfig.valueMin = 0;
	pConfig.valueMax = _world->GetSizeZ();
	pConfig.resolutionX = _world->GetSizeX();
	pConfig.resolutionY = _world->GetSizeY();
	pConfig.octaveCount = 6;

    Perlin perlin(pConfig);
	perlin.apply();
   	ImageData& seed = perlin.getResult();

    for(imp::Uint32 x = 0; x<_world->GetSizeX(); ++x)
    for(imp::Uint32 y = 0; y<_world->GetSizeY(); ++y)
    {
        int value = seed.getPixel(x,y).red;

        imp::Int32 localElevation = (imp::Int32)value;//Rand(height*3/4,height);

        imp::Int32 z = 0;
        for(; z<localElevation-1; ++z)
            _world->SetValue(x,y,(imp::Uint32)z, 4);

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
