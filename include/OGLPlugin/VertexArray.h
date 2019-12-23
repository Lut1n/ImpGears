#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <Core/Object.h>
#include <Geometry/Geometry.h>

#include <OGLPlugin/GlError.h>


IMPGEARS_BEGIN

class IMP_API VertexArray : public Object
{
public:

    Meta_Class(VertexArray)

    const int MAX_VBO_COUNT = 5;

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

    static std::uint32_t s_count() { return _s_count; }
    static std::uint32_t s_vboCount() { return _s_vboCount; }

private:

    GLuint _vao;
    std::vector<GLuint> _buffers;
    int _count;
    bool _useElements;
    Geometry::Primitive _primitive;

    static std::uint32_t _s_count;
    static std::uint32_t _s_vboCount;

};


IMPGEARS_END

#endif // VERTEXARRAY_H
