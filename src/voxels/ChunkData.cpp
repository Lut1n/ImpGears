#include "voxels/ChunkData.h"

//--------------------------------------------------------------
#define COORD_TO_INDEX(x,y,z) (x*CHUNK_SURFACE + y*CHUNK_DIM + z)

IMPGEARS_BEGIN

//--------------------------------------------------------------
ChunkData::ChunkData(const imp::Vector3& _position):
    position(_position)
{
}

//--------------------------------------------------------------
ChunkData::~ChunkData()
{
}

//--------------------------------------------------------------
const imp::Vector3& ChunkData::GetPosition()
{
    return position;
}

//--------------------------------------------------------------
imp::Uint8 ChunkData::GetValue(imp::Uint8 _x, imp::Uint8 _y, imp::Uint8 _z)
{
    return values[COORD_TO_INDEX(_x,_y,_z)];
}

//--------------------------------------------------------------
void ChunkData::SetValue(imp::Uint8 _x, imp::Uint8 _y, imp::Uint8 _z, imp::Uint8 _value)
{
    values[COORD_TO_INDEX(_x,_y,_z)] = _value;
}

//--------------------------------------------------------------
void ChunkData::Load(Parser* _parser)
{
    for(imp::Uint32 x = 0; x<CHUNK_DIM; ++x)
        for(imp::Uint32 y = 0; y<CHUNK_DIM; ++y)
    {
        imp::Uint32 toRead = CHUNK_DIM;
        while( toRead > 0 )
            toRead -= _parser->Read(&values[COORD_TO_INDEX(x,y,CHUNK_DIM-toRead)], toRead);
    }
}

//--------------------------------------------------------------
void ChunkData::Save(Parser* _parser)
{
    for(imp::Uint32 x = 0; x<CHUNK_DIM; ++x)
        for(imp::Uint32 y = 0; y<CHUNK_DIM; ++y)
        _parser->Write(&values[COORD_TO_INDEX(x,y,0)], CHUNK_DIM);
}

IMPGEARS_END
