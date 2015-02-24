#include "mesh/MeshModel.h"
#include <cstring>
#include <cstdio>
#include "graphics/GLcommon.h"

IMPGEARS_BEGIN

MeshModel::MeshModel():
    m_vertexBuffer(IMP_NULL),
    m_texCoordBuffer(IMP_NULL),
    m_normalBuffer(IMP_NULL),
    m_vertexBufferSize(0),
    m_texCoordBufferSize(0),
    m_normalBufferSize(0),
    m_vertexMode(VertexMode_Triangles),
    m_vertexSize(3)
{
}

MeshModel::~MeshModel()
{
    destroy();
}

void MeshModel::clearVertexBuffer()
{
    if(m_vertexBufferSize == 0)
        return;

    delete [] m_vertexBuffer;
    m_vertexBuffer = IMP_NULL;
    m_vertexBufferSize = 0;
}

void MeshModel::clearTexCoordBuffer()
{
    if(m_texCoordBufferSize == 0)
        return;

    delete [] m_texCoordBuffer;
    m_texCoordBuffer = IMP_NULL;
    m_texCoordBufferSize = 0;
}

void MeshModel::clearNormalBuffer()
{
    if(m_normalBufferSize == 0)
        return;

    delete [] m_normalBuffer;
    m_normalBuffer = IMP_NULL;
    m_normalBufferSize = 0;
}

void MeshModel::setVertexBuffer(const float* vertexBuffer, Uint32 size)
{
    clearVertexBuffer();

    if(size == 0)
        return;

    m_vertexBufferSize = size;
    m_vertexBuffer = new float[size];
    memcpy(m_vertexBuffer, vertexBuffer, sizeof(float)*size);
}

void MeshModel::setTexCoordBuffer(const float* texCoordBuffer, Uint32 size)
{
    clearTexCoordBuffer();

    if(size == 0)
        return;

    m_texCoordBufferSize = size;
    m_texCoordBuffer = new float[size];
    memcpy(m_texCoordBuffer, texCoordBuffer, sizeof(float)*size);
}

void MeshModel::setNormalBuffer(const float* normalBuffer, Uint32 size)
{
    clearNormalBuffer();

    if(size == 0)
        return;

    m_normalBufferSize = size;
    m_normalBuffer = new float[size];
    memcpy(m_normalBuffer, normalBuffer, sizeof(float)*size);
}

void MeshModel::setMaterial()
{

}

void MeshModel::setVertexMode(VertexMode vertexMode)
{
    m_vertexMode = vertexMode;
    switch(m_vertexMode)
    {
        case VertexMode_Points:
            m_vertexSize = 1;
            break;
        case VertexMode_Lines:
            m_vertexSize = 2;
            break;
        case VertexMode_Triangles:
            m_vertexSize = 3;
            break;
        case VertexMode_Quads:
            m_vertexSize = 4;
            break;
    }
}

void MeshModel::updateVBO(bool clearLocalData)
{
    Uint32 vboSize = (m_vertexBufferSize+m_texCoordBufferSize+m_normalBufferSize)*sizeof(float);

    if(getVBOID() == 0)
    {
        requestVBO( vboSize );
    }
    else if(getVBOSize() != vboSize)
    {
        resizeVBO(vboSize);
    }

    m_vertexOffset = 0;
    m_texCoordOffset = m_vertexBufferSize*sizeof(float);
    m_normalOffset = m_texCoordOffset + m_texCoordBufferSize*sizeof(float);

    setData(m_vertexBuffer, m_vertexBufferSize*sizeof(float), m_vertexOffset);
    setData(m_texCoordBuffer, m_texCoordBufferSize*sizeof(float), m_texCoordOffset);
    setData(m_normalBuffer, m_normalBufferSize*sizeof(float), m_normalOffset);

    if(clearLocalData)
    {
        clearVertexBuffer();
        clearTexCoordBuffer();
        clearNormalBuffer();
    }
}

void MeshModel::render()
{
    //setVertexMode(VertexMode_Lines);

    bindVBO(*this);
    enableVertexArray(m_vertexOffset);
    enableTexCoordArray(m_texCoordOffset);
    enableNormalArray(m_normalOffset);

    ///texture
    /*if(m_material != IMP_NULL)
    {
        bind();
    }*/

    Uint32 mode = GL_QUADS;
    switch(m_vertexMode)
    {
        case VertexMode_Points:
            mode = GL_POINTS;
            break;
        case VertexMode_Lines:
            mode = GL_LINES;
            break;
        case VertexMode_Triangles:
            mode = GL_TRIANGLES;
            break;
        case VertexMode_Quads:
            mode = GL_QUADS;
            break;
    }

    Uint32 count = m_vertexBufferSize/m_vertexSize;

    glDrawArrays(mode, 0, count);

    ///texture
    /*if(m_material != IMP_NULL)
    {
        unbind();
    }*/

    unbindVBO();
}

void MeshModel::destroy()
{
    clearVertexBuffer();
    clearTexCoordBuffer();
    clearNormalBuffer();
    //mat

    if( getVBOID() != 0 )
        releaseVBO();
}

void MeshModel::debugPrint()
{
    for(unsigned int i=0; i<m_vertexBufferSize; ++i)
    {
        fprintf(stdout, "%f ", m_vertexBuffer[i]);
    }

    fprintf(stdout, "\n");
}

IMPGEARS_END
