#include "IGPattern.h"

IGPattern::IGPattern()
{
	sizeX = sizeY = sizeZ = 0;
	data = IMP_NULL;
}

IGPattern::~IGPattern()
{
	if(data != IMP_NULL)
	delete [] data;
}

void IGPattern::Load(IGParser* _parser)
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

void IGPattern::Save(IGParser* _parser)
{

}
