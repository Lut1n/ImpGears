#ifndef IGWORLD_H
#define IGWORLD_H

#include "../base/IGLock.h"
#include "../base/impBase.hpp"
#include "../base/Vector3.h"
#include "../io/IGStreamable.h"
#include "scene/SceneNode.h"
#include "ChunkData.h"

class VBOChunk;

/// \brief Defines a world made of voxels.
/// IGWorld uses ChunkData in order to separate world data in multiple chunks and VBOChunk for their rendering.
/// On a value change, it makes a redirection to the good chunk and updates VBO if necessary.
class IGWorld : public IGStreamable, public SceneNode
{
private:
    imp::Uint32 sizeX, sizeY, sizeZ;
    imp::Uint32 chunkCount;

    ChunkData** data;
    VBOChunk* chunkVBOs;

    imp::Lock dataLock;

    bool valueChangedLock;

    imp::Uint32 dirtChunks[30];
    imp::Uint32 dirtChunksCount;

public:
    IGWorld(imp::Uint32 _sizeX, imp::Uint32 _sizeY, imp::Uint32 _sizeZ);

    virtual ~IGWorld();

    imp::Uint32 GetSizeX();
    imp::Uint32 GetSizeY();
    imp::Uint32 GetSizeZ();

    imp::Uint8 GetValue(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z);
    void SetValue(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z, imp::Uint8 _value);

    virtual void Load(IGParser* _parser);
    virtual void Save(IGParser* _parser);

    void UpdateAll();
    void UpdateChunk(imp::Uint32 _index);
    void UpdateChunk(imp::Uint32 _x, imp::Uint32 _y, imp::Uint32 _z);

    virtual void render(imp::Uint32 passID);

    void OnValueChangedBegin();
    void OnValueChangedEnd();
    void OnValueChanged(imp::Int32 _x, imp::Int32 _y, imp::Int32 _z, imp::Uint8 _value);
};

#endif // IGWORLD_H
