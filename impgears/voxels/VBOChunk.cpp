#include "VBOChunk.h"
#include "camera/SceneCamera.h"
#include "graphics/GLcommon.h"

imp::Texture* VBOChunk::atlasTex = 0;

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
void VBOChunk::UpdateBuffer(ChunkData* _chunk, IGWorld* _world)
{
    chunk = _chunk;

    FloatBuffer vertexBuffer;
    FloatBuffer& textCoordsBuffer = vertexBuffer;

    vertexBuffer.reserve(0.5f*CHUNK_VOLUME*6*4*(3+2)); //reserve 50%

    const imp::Vector3& position = _chunk->GetPosition();

    for(imp::Uint8 x = 0; x<CHUNK_DIM; ++x)
    for(imp::Uint8 y = 0; y<CHUNK_DIM; ++y)
    for(imp::Uint8 z = 0; z<CHUNK_DIM; ++z)
    {

        imp::Uint8 texIndex = _chunk->GetValue(x,y,z);
        imp::Uint8 other = 0;

        if(texIndex == 0)
            continue;

        imp::Int32 absX = position.getX()+(imp::Int32)x;
        imp::Int32 absY = position.getY()+(imp::Int32)y;
        imp::Int32 absZ = position.getZ()+(imp::Int32)z;

        other = _world->GetValue(absX-1, absY, absZ);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_LEFT, texIndex/*-1*/);

        other = _world->GetValue(absX+1, absY, absZ);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_RIGHT, texIndex/*-1*/);

        other = _world->GetValue(absX, absY-1, absZ);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_BACK, texIndex/*-1*/);

        other = _world->GetValue(absX, absY+1, absZ);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_FRONT, texIndex/*-1*/);

        other = _world->GetValue(absX, absY, absZ-1);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_BOTTOM, texIndex/*-1*/);

        other = _world->GetValue(absX, absY, absZ+1);
        if(other == 0)
            AddVboFace(vertexBuffer, textCoordsBuffer, x,y,z,CubeFaces_TOP, texIndex/*-1*/);
    }

    BuildBuffer(vertexBuffer, textCoordsBuffer);

	vertexBuffer.clear();
	textCoordsBuffer.clear();
}

//--------------------------------------------------------------
void VBOChunk::AddVboFace(FloatBuffer& _vertex, FloatBuffer& _textCoords,
                          float _x, float _y, float _z, CubeFaces _face, imp::Uint32 _texIndex)
{
    switch(_face)
    {
        case CubeFaces_LEFT:
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_RIGHT:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
        break;
        case CubeFaces_TOP:
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_BOTTOM:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
        break;
        case CubeFaces_FRONT:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 0, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y+VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
        break;
        case CubeFaces_BACK:
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 1, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z+VOXEL_RADIUS);
            AddTexCoord(_textCoords, 2, _texIndex);
            AddVertex(_vertex, _x-VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
            AddTexCoord(_textCoords, 3, _texIndex);
            AddVertex(_vertex, _x+VOXEL_RADIUS, _y-VOXEL_RADIUS, _z-VOXEL_RADIUS);
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
void VBOChunk::BuildBuffer(FloatBuffer& _vertex, FloatBuffer& _textCoords)
{
    if(getVBOID() == 0)
    {
        requestVBO(_vertex.size()*sizeof(float));
    }
    else if(getVBOSize() != _vertex.size()*sizeof(float))
    {
        resizeVBO(_vertex.size()*sizeof(float));
    }

	// releaseVBO();
    // requestVBO(_vertex.size()*sizeof(float));
    setData(_vertex.data(), _vertex.size()*sizeof(float));
}

//--------------------------------------------------------------
void VBOChunk::Render()
{
    if(chunk == IMP_NULL)
        return;

    const imp::Vector3& position = chunk->GetPosition();

    if(getVBOSize() > 0
       && imp::Camera::getActiveCamera()->testFov(position.getX(),position.getY(),position.getZ(),CHUNK_DIM/2) )
    {
        glPushMatrix();
        glTranslatef(position.getX(),position.getY(),position.getZ());

        glBindBuffer(GL_ARRAY_BUFFER, getVBOID());

        ///vertex
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 5*sizeof(GL_FLOAT), (char*)NULL+0);

        ///texture coord
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer(2,GL_FLOAT, 5*sizeof(GL_FLOAT), (char*)(3*sizeof(GL_FLOAT)) );

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        ///texture
        atlasTex->bind();

        glEnable(GL_CULL_FACE);
        int count = getVBOSize()/sizeof(float);
        count /= 5;
		#ifdef GRID_DEBUG
        glDrawArrays(GL_LINES, 0, count);
        #else
        glDrawArrays(GL_QUADS, 0, count);
        #endif

        ///texture
        atlasTex->unbind();

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
    }
}
