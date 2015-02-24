#include "voxels/pattern/IGPattern.h"

IMPGEARS_BEGIN

Pattern::Pattern()
{
	sizeX = sizeY = sizeZ = 0;
	data = IMP_NULL;
}

Pattern::~Pattern()
{
	if(data != IMP_NULL)
	delete [] data;
}

void Pattern::Load(imp::Parser* _parser)
{
	imp::Int8 size[3];
	_parser->Read(size, 3);
	sizeX = size[0];
	sizeY = size[1];
	sizeZ = size[2];

	int count = sizeX*sizeY*sizeZ;
	data = new imp::Uint8[count];
	_parser->Read(data, count);
}

void Pattern::Save(imp::Parser* _parser)
{

}

IMPGEARS_END
