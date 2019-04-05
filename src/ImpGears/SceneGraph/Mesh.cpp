#include <SceneGraph/Mesh.h>
#include <cstring>
#include <cstdio>
#include <SceneGraph/OpenGL.h>

IMPGEARS_BEGIN

Mesh::Mesh():
    m_vertexBuffer(nullptr),
    m_texCoordBuffer(nullptr),
    m_normalBuffer(nullptr),
    m_vertexBufferSize(0),
    m_texCoordBufferSize(0),
    m_normalBufferSize(0),
    m_vertexMode(VertexMode_Triangles),
    m_vertexSize(3)
{
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::clearVertexBuffer()
{
    if(m_vertexBufferSize == 0)
        return;

    delete [] m_vertexBuffer;
    m_vertexBuffer = nullptr;
    m_vertexBufferSize = 0;
}

void Mesh::clearTexCoordBuffer()
{
    if(m_texCoordBufferSize == 0)
        return;

    delete [] m_texCoordBuffer;
    m_texCoordBuffer = nullptr;
    m_texCoordBufferSize = 0;
}

void Mesh::clearNormalBuffer()
{
    if(m_normalBufferSize == 0)
        return;

    delete [] m_normalBuffer;
    m_normalBuffer = nullptr;
    m_normalBufferSize = 0;
}

void Mesh::setVertexBuffer(const float* vertexBuffer, std::uint32_t size)
{
    clearVertexBuffer();

    if(size == 0)
        return;

    m_vertexBufferSize = size;
    m_vertexBuffer = new float[size];
    memcpy(m_vertexBuffer, vertexBuffer, sizeof(float)*size);
}

void Mesh::setTexCoordBuffer(const float* texCoordBuffer, std::uint32_t size)
{
    clearTexCoordBuffer();

    if(size == 0)
        return;

    m_texCoordBufferSize = size;
    m_texCoordBuffer = new float[size];
    memcpy(m_texCoordBuffer, texCoordBuffer, sizeof(float)*size);
}

void Mesh::setNormalBuffer(const float* normalBuffer, std::uint32_t size)
{
    clearNormalBuffer();

    if(size == 0)
        return;

    m_normalBufferSize = size;
    m_normalBuffer = new float[size];
    memcpy(m_normalBuffer, normalBuffer, sizeof(float)*size);
}

void Mesh::setMaterial()
{

}

void Mesh::setVertexMode(VertexMode vertexMode)
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

void Mesh::updateVBO(bool clearLocalData)
{
    int vboSize = (m_vertexBufferSize+m_texCoordBufferSize+m_normalBufferSize)*sizeof(float);

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

void Mesh::render()
{
    //setVertexMode(VertexMode_Lines);

    bindVBO(*this);
    enableVertexArray(m_vertexOffset);
    enableTexCoordArray(m_texCoordOffset);
    enableNormalArray(m_normalOffset);

    ///texture
    /*if(m_material != nullptr)
    {
        bind();
    }*/

    std::uint32_t mode = GL_QUADS;
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

    std::uint32_t count = m_vertexBufferSize/3;

    glDrawArrays(mode, 0, count);

    ///texture
    /*if(m_material != nullptr)
    {
        unbind();
    }*/

    unbindVBO();
}

void Mesh::destroy()
{
    clearVertexBuffer();
    clearTexCoordBuffer();
    clearNormalBuffer();
    //mat

    if( getVBOID() != 0 )
        releaseVBO();
}

void Mesh::debugPrint()
{
    for(unsigned int i=0; i<m_vertexBufferSize; ++i)
    {
        fprintf(stdout, "%f ", m_vertexBuffer[i]);
    }

    fprintf(stdout, "\n");
}

IMPGEARS_END
