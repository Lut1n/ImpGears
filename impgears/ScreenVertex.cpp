#include "ScreenVertex.h"

#include "graphics/GLcommon.h"

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
    glBindBuffer(GL_ARRAY_BUFFER, getVBOID());

    ///vertex
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL+0);
    GL_CHECKERROR("vertex pointer");

    ///texture coord
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(m_texCoordOffset));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

IMPGEARS_END
