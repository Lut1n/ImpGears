#include "VoxelWorld.h"
#include "VBOChunk.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
VoxelWorld::VoxelWorld(imp::Uint32 _sizeX, imp::Uint32 _sizeY, imp::Uint32 _sizeZ):
    sizeX(_sizeX),
    sizeY(_sizeY),
    sizeZ(_sizeZ)
{
    valueChangedLock = false;
    dirtChunksCount = 0;

    imp::Uint32 chunkSizeX = sizeX/CHUNK_DIM;
    imp::Uint32 chunkSizeY = sizeY/CHUNK_DIM;
    imp::Uint32 chunkSizeZ = sizeZ/CHUNK_DIM;

	chunkCount = chunkSizeX*chunkSizeY*chunkSizeZ;

	fprintf(stdout, "chunk count = %d\n", chunkCount);

    data = new ChunkData*[chunkCount];
    chunkVBOs = new VBOChunk[chunkCount];

	int i=0;
    for(imp::Uint32 z = 0; z<chunkSizeZ; ++z)
    for(imp::Uint32 x = 0; x<chunkSizeX; ++x)
    for(imp::Uint32 y = 0; y<chunkSizeY; ++y)
    {
        imp::Vector3 position((float)x*CHUNK_DIM,(float)y*CHUNK_DIM,(float)z*CHUNK_DIM);
        data[x*chunkSizeY*chunkSizeZ + y*chunkSizeZ + z] = new ChunkData(position);
        ++i;
    }
}

//--------------------------------------------------------------
VoxelWorld::~VoxelWorld()
{
    delete[] chunkVBOs;

    imp::Uint32 dataSize = sizeX*sizeY*sizeZ;

    for(imp::Uint32 index = 0; index<dataSize; ++index)
        if(data[index] != IMP_NULL)
            delete data[index];

    delete [] data;
}

//--------------------------------------------------------------
imp::Uint32 VoxelWorld::GetSizeX()
{
    return sizeX;
}

//--------------------------------------------------------------
imp::Uint32 VoxelWorld::GetSizeY()
{
    return sizeY;
}

//--------------------------------------------------------------
imp::Uint32 VoxelWorld::GetSizeZ()
{
    return sizeZ;
}

//--------------------------------------------------------------
imp::Uint8 VoxelWorld::GetValue(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z)
{
    if( _x<0 || _y<0 || _z<0 || _x>=(int)sizeX || _y>=(int)sizeY || _z>=(int)sizeZ )
        return 0;

    //imp::Uint32 chunkSizeX = sizeX/CHUNK_DIM;
    imp::Uint32 chunkSizeY = sizeY/CHUNK_DIM;
    imp::Uint32 chunkSizeZ = sizeZ/CHUNK_DIM;

    imp::Int32 chunkX = _x/CHUNK_DIM;
    imp::Int32 chunkY = _y/CHUNK_DIM;
    imp::Int32 chunkZ = _z/CHUNK_DIM;

    ChunkData* chunk = data[chunkX*chunkSizeY*chunkSizeZ+chunkY*chunkSizeZ+chunkZ];

    imp::Uint8 subDataX = _x - (chunkX*CHUNK_DIM);
    imp::Uint8 subDataY = _y - (chunkY*CHUNK_DIM);
    imp::Uint8 subDataZ = _z - (chunkZ*CHUNK_DIM);

    return chunk->GetValue(subDataX, subDataY, subDataZ);
}

//--------------------------------------------------------------
void VoxelWorld::SetValue(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z, imp::Uint8 _value)
{

    //fprintf(stdout, "%d;%d;%d setv %u\n", _x,_y,_z,_value);

    if( _x<0 || _y<0 || _z<0 || _x>=(int)sizeX || _y>=(int)sizeY || _z>=(int)sizeZ )
        return;

    //imp::Uint32 chunkSizeX = sizeX/CHUNK_DIM;
    imp::Uint32 chunkSizeY = sizeY/CHUNK_DIM;
    imp::Uint32 chunkSizeZ = sizeZ/CHUNK_DIM;

    imp::Int32 chunkX = _x/CHUNK_DIM;
    imp::Int32 chunkY = _y/CHUNK_DIM;
    imp::Int32 chunkZ = _z/CHUNK_DIM;

    ChunkData* chunk = data[chunkX*chunkSizeY*chunkSizeZ+chunkY*chunkSizeZ+chunkZ];

    imp::Uint8 subDataX = _x - (chunkX*CHUNK_DIM);
    imp::Uint8 subDataY = _y - (chunkY*CHUNK_DIM);
    imp::Uint8 subDataZ = _z - (chunkZ*CHUNK_DIM);

    chunk->SetValue(subDataX, subDataY, subDataZ, _value);
}

//--------------------------------------------------------------
void VoxelWorld::Load(Parser* _parser)
{
    imp::Uint32 dataSize = sizeX*sizeY*sizeZ;

    for(imp::Uint32 index = 0; index<dataSize; ++index)
        data[index]->Load(_parser);
}

//--------------------------------------------------------------
void VoxelWorld::Save(Parser* _parser)
{
    imp::Uint32 dataSize = sizeX*sizeY*sizeZ;

    for(imp::Uint32 index = 0; index<dataSize; ++index)
        data[index]->Save(_parser);
}

//--------------------------------------------------------------
void VoxelWorld::UpdateAll()
{
    for(imp::Uint32 index = 0; index<chunkCount; ++index)
        UpdateChunk(index);
}

//--------------------------------------------------------------
void VoxelWorld::UpdateChunk(imp::Uint32 _index)
{
    if(valueChangedLock)
        dirtChunks[dirtChunksCount++] = _index;
    else
        chunkVBOs[_index].UpdateBuffer(data[_index], this);
}

//--------------------------------------------------------------
void VoxelWorld::UpdateChunk(imp::Uint32 _x, imp::Uint32 _y, imp::Uint32 _z)
{
	imp::Uint32 chunkX = _x/CHUNK_DIM;
	imp::Uint32 chunkY = _y/CHUNK_DIM;
	imp::Uint32 chunkZ = _z/CHUNK_DIM;

    imp::Uint32 index = 0;

    //find chunk
    bool found = false;
    for(index=0; index<chunkCount; ++index)
    {
        const imp::Vector3& position = data[index]->GetPosition();
        if(position.getX() == chunkX*CHUNK_DIM &&
           position.getY() == chunkY*CHUNK_DIM &&
           position.getZ() == chunkZ*CHUNK_DIM   )
        {
            found = true;
            break;
        }
    }

    if(found)
    {
        UpdateChunk(index);
    }
}

//--------------------------------------------------------------
void VoxelWorld::render(imp::Uint32 passID)
{
	dataLock.lock();

    for(imp::Uint32 index = 0; index<chunkCount; ++index)
    {
        chunkVBOs[index].render(passID);
    }

	dataLock.unlock();
}

//--------------------------------------------------------------
void VoxelWorld::OnValueChangedBegin()
{
    valueChangedLock = true;
}

//--------------------------------------------------------------
void VoxelWorld::OnValueChangedEnd()
{
    if(valueChangedLock)
    {
        valueChangedLock = false;

        for(unsigned int i=0; i<dirtChunksCount; ++i)
            UpdateChunk(dirtChunks[i]);

        dirtChunksCount = 0;
    }
}

//--------------------------------------------------------------
void VoxelWorld::OnValueChanged(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z, imp::Uint8 _value)
{
	dataLock.lock();

	SetValue(_x,_y,_z,_value);
	UpdateChunk(_x,_y,_z);

	imp::Int32 chunkX =  _x/CHUNK_DIM;
	imp::Int32 chunkY =  _y/CHUNK_DIM;
	imp::Int32 chunkZ =  _z/CHUNK_DIM;

	imp::Int32 innerX = _x - (chunkX*CHUNK_DIM);
	imp::Int32 innerY = _y - (chunkY*CHUNK_DIM);
	imp::Int32 innerZ = _z - (chunkZ*CHUNK_DIM);

	if(innerX==0)
		UpdateChunk(_x-1,_y,_z);
	else if(innerX==CHUNK_DIM-1)
		UpdateChunk(_x+1,_y,_z);

	if(innerY==0)
		UpdateChunk(_x,_y-1,_z);
	else if(innerY==CHUNK_DIM-1)
		UpdateChunk(_x,_y+1,_z);

	if(innerZ==0)
		UpdateChunk(_x,_y,_z-1);
	else if(innerZ==CHUNK_DIM-1)
		UpdateChunk(_x,_y,_z+1);

	dataLock.unlock();
}

IMPGEARS_END
