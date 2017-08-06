/*
Data/VBOChunk.h

Define a VBO for a world chunk.

Provides functions for building and rendering a chunk.
During the building, the culling method is to ignore the two hidden faces of two adjacent voxels.

The next step is to reduce the number of rendered faces by choosing a simpler mesh for corners.
*/

#ifndef VBOCHUNK_H
#define VBOCHUNK_H

#include <vector>

#include "Core/impBase.h"
#include "Math/Matrix4.h"

#include "Graphics/VBOData.h"
#include "Graphics/Texture.h"
#include "Data/VoxelWorld.h"
#include "Data/ChunkData.h"

#include "Graphics/SceneNode.h"

#define VOXEL_RADIUS 0.5f

//#define GRID_DEBUG

IMPGEARS_BEGIN

typedef std::vector<float> FloatBuffer;

class VoxelWorld;

class IMP_API VBOChunk : public imp::VBOData, public SceneNode
{
private:

    enum CubeFaces
    {
       CubeFaces_TOP = (1 << 0),
       CubeFaces_BOTTOM = (1 << 1),
       CubeFaces_FRONT = (1 << 2),
       CubeFaces_BACK = (1 << 3),
       CubeFaces_LEFT = (1 << 4),
       CubeFaces_RIGHT = (1 << 5),
       CubeFaces_DIAG1 = (1 << 6),
       CubeFaces_DIAG2 = (1 << 7),
       CubeFaces_ALL = (1 << 8) //all
    };

    ChunkData* chunk;

    void AddVboFace(FloatBuffer& _vertex, FloatBuffer& _normals, FloatBuffer& _textCoords,
                    float _x, float _y, float _z, CubeFaces _face, imp::Uint32 _texIndex);

    void AddVertex(FloatBuffer& _vertex, float _x, float _y, float _z);
    void AddNormal(FloatBuffer& _normals, float _x, float _y, float _z);
    void AddTexCoord(FloatBuffer& _texCoords, imp::Uint8 _corner, imp::Uint8 _texIndex);

    void BuildBuffer(FloatBuffer& _vertex, FloatBuffer& _normals, FloatBuffer& _textCoords);

public:

    VBOChunk();
    virtual ~VBOChunk();

    void UpdateBuffer(ChunkData* _chunk, VoxelWorld* _world);

    void render(imp::Uint32 passID);

    imp::Uint64 m_texCoordOffset, m_normalOffset;
};

IMPGEARS_END

#endif // VBOCHUNK_H
