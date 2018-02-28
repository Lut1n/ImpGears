#ifndef IMP_MESH_H
#define IMP_MESH_H

#include <Core/Object.h>
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

        void setVertexBuffer(const float* vertexBuffer, std::uint32_t size);
        void setTexCoordBuffer(const float* texCoordBuffer, std::uint32_t size);
        void setNormalBuffer(const float* normalBuffer, std::uint32_t size);

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

    std::uint32_t m_vertexBufferSize;
    std::uint32_t m_texCoordBufferSize;
    std::uint32_t m_normalBufferSize;

    std::uint64_t m_vertexOffset;
    std::uint64_t m_texCoordOffset;
    std::uint64_t m_normalOffset;

    VertexMode m_vertexMode;
    std::uint32_t m_vertexSize;

    //Material* m_material;
};

IMPGEARS_END

#endif // IMP_MESH_H
