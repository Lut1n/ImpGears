/*
voxels/ChunkData.h

Define a data chunk of a world. A ChunkData is streamable and directly used by a IGWorld object.
*/

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include "../base/impBase.hpp"
#include "../base/Vector3.h"
#include "../io/IGStreamable.h"

#define CHUNK_DIM 32
#define CHUNK_SURFACE CHUNK_DIM*CHUNK_DIM
#define CHUNK_VOLUME CHUNK_SURFACE*CHUNK_DIM

class ChunkData : IGStreamable
{
private:

    imp::Uint8 values[CHUNK_VOLUME];

    imp::Vector3 position;

public:
    ChunkData(const imp::Vector3& _position);

    virtual ~ChunkData();

    const imp::Vector3& GetPosition();

    imp::Uint8 GetValue(imp::Uint8 _x, imp::Uint8 _y, imp::Uint8 _z);
    void SetValue(imp::Uint8 _x, imp::Uint8 _y, imp::Uint8 _z, imp::Uint8 _value);

    virtual void Load(IGParser* _parser);
    virtual void Save(IGParser* _parser);
};

#endif // CHUNKDATA_H
