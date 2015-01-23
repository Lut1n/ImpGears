#ifndef IMP_MESHMODEL_H
#define IMP_MESHMODEL_H

#include "base/impBase.hpp"
#include "graphics/VBOData.h"

IMPGEARS_BEGIN

class MeshModel : public VBOData
{
    public:

        enum VertexMode
        {
            VertexMode_Triangles = 0,
            VertexMode_Quads,
            VertexMode_Lines,
            VertexMode_Points
        };

        MeshModel();
        virtual ~MeshModel();

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

    VertexMode m_vertexMode;
    Uint32 m_vertexSize;

    //Material* m_material;
};

IMPGEARS_END

#endif // IMP_MESHMODEL_H
