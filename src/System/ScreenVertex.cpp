#include "System/ScreenVertex.h"

#include "Graphics/GLcommon.h"

IMPGEARS_BEGIN

ScreenVertex::ScreenVertex()
{
    const float vertex[12] =
    {
      0.f, 0.f, 0.f,
      1.f, 0.f, 0.f,
      1.f, 1.f, 0.f,
      0.f, 1.f, 0.f
    };

    const float texCoord[8] =
    {
      0.f, 0.f,
      1.f, 0.f,
      1.f, 1.f,
      0.f, 1.f
    };

    Uint32 vertexBuffSize = 12*sizeof(float);
    Uint32 texCoordSize = 8*sizeof(float);

    m_texCoordOffset = vertexBuffSize;

    requestVBO(vertexBuffSize+texCoordSize);
    setData(vertex, vertexBuffSize, 0);
    setData(texCoord, texCoordSize, m_texCoordOffset);
}

ScreenVertex::~ScreenVertex()
{
}


void ScreenVertex::render(imp::Uint32 passID)
{
    bindVBO(*this);

    ///vertex
    enableVertexArray(0);
    GL_CHECKERROR("vertex pointer");

    ///texture coord
    enableTexCoordArray(m_texCoordOffset);

    glDrawArrays(GL_QUADS, 0, 4);

    unbindVBO();
}

IMPGEARS_END
