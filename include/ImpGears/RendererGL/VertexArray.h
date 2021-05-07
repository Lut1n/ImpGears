#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/Geometry/InstancedGeometry.h>
#include <RendererGL/Export.h>

#include <RendererGL/GlError.h>


IMPGEARS_BEGIN

class IG_BKND_GL_API VertexArray : public Object
{
public:

    Meta_Class(VertexArray)

    enum UsageMode
    {
        UsageMode_Static = 0,
        UsageMode_Dynamic
    };

    enum BufferName
    {
        Buffer_Indices = 0,
        Buffer_Vertices,
        Buffer_Colors,
        Buffer_Normals,
        Buffer_TexCoords,
        Buffer_InstTransforms,

        // buffer vector size
        Buffer_Count
    };

    VertexArray();

    virtual ~VertexArray();

    template<typename T>
    void loadBuffer(BufferName bufName, const std::vector<T>& buf, bool is_indices = false)
    {
        use();
        int size = buf.size() * sizeof(T);
        GLenum target = is_indices? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
        glBindBuffer(target, _buffers[bufName]);
        GL_CHECKERROR("VertexArray::loadBuffer bind");
        glBufferData(target, size, buf.data(), GL_STATIC_DRAW);
        GL_CHECKERROR("VertexArray::loadBuffer data");
    }

    template<typename T, int N, typename V>
    void fillBuffer(std::vector<T>& buff, const std::vector<V>& vbuff)
    {
        for(auto v : vbuff)
        {
            for(int i=0;i<N;++i) buff.push_back( v[i] );
        }
    }

    void load(const Geometry& geometry);

    void enableAttrib(BufferName bufName, GLuint location, int size, int stride, int offset);

    void use();

    void draw();

    static std::uint32_t s_count();
    static std::uint32_t s_vboCount();

private:

    GLuint _vao;
    std::vector<GLuint> _buffers;
    int _count;
    bool _useElements;
    Geometry::Primitive _primitive;

    bool _instanced;
    int _instanceCount;
};


IMPGEARS_END

#endif // VERTEXARRAY_H
