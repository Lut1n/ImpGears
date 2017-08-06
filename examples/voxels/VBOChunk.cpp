#include <Graphics/Camera.h>
#include <Graphics/GLcommon.h>
#include <Graphics/DefaultShader.h>
#include <Graphics/GraphicRenderer.h>
#include <Math/Matrix3.h>

#include "VBOChunk.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
VBOChunk::VBOChunk()
{
    chunk = IMP_NULL;
}

//--------------------------------------------------------------
VBOChunk::~VBOChunk()
{
}

//--------------------------------------------------------------
void VBOChunk::UpdateBuffer(ChunkData* _chunk, VoxelWorld* _world)
{
    chunk = _chunk;
    const imp::Vector3& position = _chunk->GetPosition();
    setPosition(position);

    FloatBuffer vertexBuffer;
    FloatBuffer normalsBuffer;
    FloatBuffer textCoordsBuffer;

    for(imp::Uint32 z = 0; z<CHUNK_DIM; ++z)
    for(imp::Uint32 x = 0; x<CHUNK_DIM; ++x)
    for(imp::Uint32 y = 0; y<CHUNK_DIM; ++y)
    {
        imp::Uint32 texIndex = _chunk->GetValue(x,y,z);
        imp::Uint32 other = 0;

        if(texIndex == 0)
            continue;

        imp::Int32 absX = position.getX()+(imp::Int32)x;
        imp::Int32 absY = position.getY()+(imp::Int32)y;
        imp::Int32 absZ = position.getZ()+(imp::Int32)z;

        other = _world->GetValue(absX-1, absY, absZ);
        if(other == 0  && texIndex == 1)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_LEFT, texIndex+1);
        else if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_LEFT, texIndex);

        other = _world->GetValue(absX+1, absY, absZ);
        if(other == 0  && texIndex == 1)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_RIGHT, texIndex+1);
        else if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_RIGHT, texIndex);

        other = _world->GetValue(absX, absY-1, absZ);
        if(other == 0  && texIndex == 1)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_BACK, texIndex+1);
        else if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_BACK, texIndex);

        other = _world->GetValue(absX, absY+1, absZ);
        if(other == 0  && texIndex == 1)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_FRONT, texIndex+1);
        else if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_FRONT, texIndex);

        other = _world->GetValue(absX, absY, absZ-1);
        if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_BOTTOM, texIndex);

        other = _world->GetValue(absX, absY, absZ+1);
        if(other == 0  && texIndex == 1)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_TOP, texIndex+2);
        else if(other == 0)
            AddVboFace(vertexBuffer, normalsBuffer, textCoordsBuffer, x,y,z,CubeFaces_TOP, texIndex);
    }

    BuildBuffer(vertexBuffer, normalsBuffer, textCoordsBuffer);
}

//--------------------------------------------------------------
void VBOChunk::AddVboFace(FloatBuffer& _vertex, FloatBuffer& _normals, FloatBuffer& _textCoords,
                          float _x, float _y, float _z, CubeFaces _face, imp::Uint32 _texIndex)
{
    switch(_face)
    {
        case CubeFaces_LEFT:
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, -1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, -1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, -1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, -1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_RIGHT:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 1.f, 0.f, 0.f);
            AddTexCoord(_textCoords, 0, _texIndex);
        break;
        case CubeFaces_TOP:
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, 1.f);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, 1.f);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, 1.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, 1.f);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_BOTTOM:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, -1.f);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, -1.f);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, -1.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 0.f, -1.f);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_FRONT:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 1.f, 0.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 1.f, 0.f);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 1.f, 0.f);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, 1.f, 0.f);
            AddTexCoord(_textCoords, 1, _texIndex);
        break;
        case CubeFaces_BACK:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, -1.f, 0.f);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddNormal(_normals, 0.f, -1.f, 0.f);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, -1.f, 0.f);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddNormal(_normals, 0.f, -1.f, 0.f);
            AddTexCoord(_textCoords, 0, _texIndex);
        break;
        case CubeFaces_DIAG1:
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_DIAG2:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        default:
        break;
    }
}

//--------------------------------------------------------------
void VBOChunk::AddVertex(FloatBuffer& _vertex, float _x, float _y, float _z)
{
    _vertex.push_back(_x);
    _vertex.push_back(_y);
    _vertex.push_back(_z);
}

//--------------------------------------------------------------
void VBOChunk::AddNormal(FloatBuffer& _normals, float _x, float _y, float _z)
{
    _normals.push_back(_x);
    _normals.push_back(_y);
    _normals.push_back(_z);
}

//--------------------------------------------------------------
void VBOChunk::AddTexCoord(FloatBuffer& _texCoords, imp::Uint8 _corner, imp::Uint8 _texIndex)
{
	///fixe
	const float CORRECTION = 0.001f;

    float left = 0.125*_texIndex;
    float  right = left + 0.125;
    float bottom = 0.875f;
    float top = 1.f;

	left += CORRECTION;
	right -= CORRECTION;
	bottom += CORRECTION;
	top -= CORRECTION;

    switch(_corner)
    {
    case 0:///bottom left
        _texCoords.push_back(left);
        _texCoords.push_back(bottom);
    break;
    case 1:///top left
        _texCoords.push_back(left);
        _texCoords.push_back(top);
    break;
    case 2:///top right
        _texCoords.push_back(right);
        _texCoords.push_back(top);
    break;
    case 3:///bottom right
        _texCoords.push_back(right);
        _texCoords.push_back(bottom);
    break;
    }
}

//--------------------------------------------------------------
void VBOChunk::BuildBuffer(FloatBuffer& _vertex, FloatBuffer& _normals, FloatBuffer& _textCoords)
{

    imp::Uint32 vboSize = (_vertex.size()+_normals.size()+_textCoords.size())*sizeof(float);


    if(/*vboSize > 0 && */getVBOID() == 0)
    {
        requestVBO(vboSize);
    }
    /*else if(vboSize == 0 && getVBOID() > 0)
    {
        releaseVBO();
        GL_CHECKERROR("release VBO");
    }*/

    if(getVBOSize() != vboSize)
    {
        resizeVBO(vboSize);
    }

    int vertexBuffSize = _vertex.size()*sizeof(float);
    int normalBuffSize = _normals.size()*sizeof(float);
    int texcooBuffSize = _textCoords.size()*sizeof(float);

    m_texCoordOffset = vertexBuffSize;
    m_normalOffset = vertexBuffSize + normalBuffSize;

    setVertices(_vertex.data(), vertexBuffSize);
    setData(_textCoords.data(), texcooBuffSize, m_texCoordOffset);
    // setData(_normals.data(), normalBuffSize, m_normalOffset);
}

//--------------------------------------------------------------
void VBOChunk::render(imp::Uint32 passID)
{
    if(chunk == IMP_NULL)
        return;

    const imp::Vector3& position = chunk->GetPosition();

    if(getVBOSize() > 0
       && imp::Camera::getActiveCamera()->testFov(position.getX(),position.getY(),position.getZ(),CHUNK_DIM/2) )
    {
        imp::DefaultShader::_instance->setMatrix4Parameter("u_model", getModelMatrix());
		drawVBO();
    }
}

IMPGEARS_END
