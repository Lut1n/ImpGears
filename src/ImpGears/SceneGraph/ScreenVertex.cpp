#include <SceneGraph/ScreenVertex.h>

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
	setVertices(vertex, vertexBuffSize);
//    setData(vertex, vertexBuffSize, 0);
    setData(texCoord, texCoordSize, m_texCoordOffset);
}

ScreenVertex::~ScreenVertex()
{
}


void ScreenVertex::render()
{
	drawVBO();
}

IMPGEARS_END
