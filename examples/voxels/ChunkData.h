#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <Core/impBase.h>
#include <Core/Vector3.h>
#include <Core/Streamable.h>

#define CHUNK_DIM 32
#define CHUNK_SURFACE CHUNK_DIM*CHUNK_DIM
#define CHUNK_VOLUME CHUNK_SURFACE*CHUNK_DIM

IMPGEARS_BEGIN

/// \brief Define a data chunk of a voxel terrain. A ChunkData is streamable and directly used by a IGWorld object.
class IMP_API ChunkData : public Streamable
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

    virtual void Load(Parser* _parser);
    virtual void Save(Parser* _parser);
};

IMPGEARS_END

#endif // CHUNKDATA_H
