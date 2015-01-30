#include "VBOChunk.h"
#include "camera/SceneCamera.h"
#include "graphics/GLcommon.h"

#include "shaders/DeferredShader.h"
#include "scene/GraphicRenderer.h"

#include "base/Matrix3.h"


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
    const imp::Vector3& position = _chunk->GetPosition();

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
    _normals.push_back( (_x+1.f)/2.f );
    _normals.push_back( (_y+1.f)/2.f );
    _normals.push_back( (_z+1.f)/2.f );
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
        GL_CHECKERROR("request VBO");
    }
    /*else if(vboSize == 0 && getVBOID() > 0)
    {
        releaseVBO();
        GL_CHECKERROR("release VBO");
    }*/

    if(getVBOSize() != vboSize)
    {
        resizeVBO(vboSize);
        GL_CHECKERROR("resize VBO");
    }

    int vertexBuffSize = _vertex.size()*sizeof(float);
    int normalBuffSize = _normals.size()*sizeof(float);
    int texcooBuffSize = _textCoords.size()*sizeof(float);

    m_normalOffset = vertexBuffSize;
    m_texCoordOffset = vertexBuffSize + normalBuffSize;

    setData(_vertex.data(), vertexBuffSize, 0);
    setData(_normals.data(), normalBuffSize, m_normalOffset);
    setData(_textCoords.data(), texcooBuffSize, m_texCoordOffset);
}

//--------------------------------------------------------------
void VBOChunk::Render(imp::Uint32 passID)
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
        glVertexPointer(3, GL_FLOAT, 0, (char*)NULL+0);

        ///normals
        glEnableClientState( GL_NORMAL_ARRAY );
        glNormalPointer(GL_FLOAT, 0, (GLvoid*)(m_normalOffset));

        ///texture coord
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(m_texCoordOffset));

        glBindBuffer(GL_ARRAY_BUFFER, 0);


        if(passID == 0)
        {
            m_shadowMvMat = imp::GraphicRenderer::getInstance()->getModelViewMatrix();
        }
        else if(passID == 3)
        {
            m_mvMat = imp::GraphicRenderer::getInstance()->getModelViewMatrix();
            imp::Matrix3 normalMat3(m_mvMat);
            imp::Mat4 normalMat4 = normalMat3.getInverse().getTranspose().asMatrix4();

            imp::DeferredShader::instance->setMat4Parameter("u_mvMat", m_mvMat);
            imp::DeferredShader::instance->setMat4Parameter("u_normalMat", normalMat4);
            imp::DeferredShader::instance->setFloatParameter("u_chunkLevel", position.getZ());
			imp::DeferredShader::instance->setMat4Parameter("u_shadowMvMat", m_shadowMvMat);
        }

        int count = m_normalOffset/sizeof(float);
        count /= 3;
		#ifdef GRID_DEBUG
        glDrawArrays(GL_LINES, 0, count);
        #else
        glDrawArrays(GL_QUADS, 0, count);
        #endif

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
    }
}
