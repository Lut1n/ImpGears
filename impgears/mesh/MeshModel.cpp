#include "MeshModel.h"
#include "cstring"

IMPGEARS_BEGIN

MeshModel::MeshModel():
    m_vertexBuffer(IMP_NULL),
    m_texCoordBuffer(IMP_NULL),
    m_normalBuffer(IMP_NULL),
    m_vertexBufferSize(0),
    m_texCoordBufferSize(0),
    m_normalBufferSize(0)
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

    m_vertexBufferSize = size;
    m_vertexBuffer = new float[size];
    memcpy(m_vertexBuffer, vertexBuffer, sizeof(float)*size);
}

void MeshModel::setTexCoordBuffer(const float* texCoordBuffer, Uint32 size)
{
    clearTexCoordBuffer();

    m_texCoordBufferSize = size;
    m_texCoordBuffer = new float[size];
    memcpy(m_texCoordBuffer, texCoordBuffer, sizeof(float)*size);
}

void MeshModel::setNormalBuffer(const float* normalBuffer, Uint32 size)
{
    clearNormalBuffer();

    m_normalBufferSize = size;
    m_normalBuffer = new float[size];
    memcpy(m_normalBuffer, normalBuffer, sizeof(float)*size);
}

void MeshModel::setMaterial()
{

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

    setData(m_vertexBuffer, m_vertexBufferSize*sizeof(float), 0);
    setData(m_texCoordBuffer, m_texCoordBufferSize*sizeof(float), m_vertexBufferSize*sizeof(float));
    setData(m_normalBuffer, m_normalBufferSize*sizeof(float), m_texCoordBufferSize*sizeof(float));

    if(clearLocalData)
    {
        clearVertexBuffer();
        clearTexCoordBuffer();
        clearNormalBuffer();
    }
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

IMPGEARS_END
