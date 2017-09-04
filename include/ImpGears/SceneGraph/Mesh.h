#ifndef IMP_MESH_H
#define IMP_MESH_H

#include "Core/impBase.h"
#include <SceneGraph/VBOData.h>

IMPGEARS_BEGIN

class IMP_API Mesh : public VBOData
{
    public:

        enum VertexMode
        {
            VertexMode_Triangles = 0,
            VertexMode_Quads,
            VertexMode_Lines,
            VertexMode_Points
        };

        Mesh();
        virtual ~Mesh();

        void clearVertexBuffer();
        void clearTexCoordBuffer();
        void clearNormalBuffer();

        void setVertexBuffer(const float* vertexBuffer, Uint32 size);
        void setTexCoordBuffer(const float* texCoordBuffer, Uint32 size);
        void setNormalBuffer(const float* normalBuffer, Uint32 size);

        void setMaterial();

        void setVertexMode(VertexMode vertexMode);
        VertexMode getVertexMode() const{return m_vertexMode;}

        void updateVBO(bool clearLocalData = false);

        void render();

        void destroy();

    protected:
    private:

    void debugPrint();

    float* m_vertexBuffer;
    float* m_texCoordBuffer;
    float* m_normalBuffer;

    Uint32 m_vertexBufferSize;
    Uint32 m_texCoordBufferSize;
    Uint32 m_normalBufferSize;

    Uint64 m_vertexOffset;
    Uint64 m_texCoordOffset;
    Uint64 m_normalOffset;

    VertexMode m_vertexMode;
    Uint32 m_vertexSize;

    //Material* m_material;
};

IMPGEARS_END

#endif // IMP_MESH_H
