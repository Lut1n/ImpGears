#include <RendererGL/VertexArray.h>

IMPGEARS_BEGIN

static std::uint32_t s_vertexArrayCount = 0;
static std::uint32_t s_vertexBufferObjectCount = 0;

//--------------------------------------------------------------
VertexArray::VertexArray()
{
    _vao = 0;
    _buffers.resize(Buffer_Count, 0);
    _primitive = Geometry::Primitive_Triangles;
    _useElements = false;
    _instanced = false;
    _count = 0;
    _instanceCount = 0;

    glGenVertexArrays(1, &_vao);
    GL_CHECKERROR("gen vao");
    s_vertexArrayCount++;

    glGenBuffers(_buffers.size(), _buffers.data());
    GL_CHECKERROR("gen vbo");
    s_vertexBufferObjectCount += _buffers.size();
}

//--------------------------------------------------------------
VertexArray::~VertexArray()
{
    use();

    glDeleteBuffers(_buffers.size(), _buffers.data());
    GL_CHECKERROR("delete vbo");
    s_vertexBufferObjectCount -= _buffers.size();

    glDeleteVertexArrays(1, &_vao);
    GL_CHECKERROR("delete vao");
    s_vertexArrayCount--;
}

//--------------------------------------------------------------
void VertexArray::load(const Geometry& geometry)
{
    _useElements = geometry._hasIndices;
    _primitive = geometry.getPrimitive();

    std::vector<float> buf; geometry.fillBuffer( buf );
    loadBuffer<float>(Buffer_Vertices, buf);
    _count = geometry.size();

    if(geometry._hasColors)
    {
        std::vector<float> buf; fillBuffer<float,3,Vec3>(buf, geometry._colors);
        loadBuffer<float>(Buffer_Colors, buf);
    }

    if(geometry._hasNormals)
    {
        std::vector<float> buf; fillBuffer<float,3,Vec3>(buf, geometry._normals);
        loadBuffer<float>(Buffer_Normals, buf);
    }

    if(geometry._hasTexCoords)
    {
        std::vector<float> buf; fillBuffer<float,2,Geometry::TexCoord>(buf, geometry._texCoords);
        loadBuffer<float>(Buffer_TexCoords, buf);
    }

    if(geometry._hasIndices)
    {
        loadBuffer<std::uint32_t>(Buffer_Indices, geometry._indices, true);
        _count = geometry._indices.size();
    }
    
    
    const InstancedGeometry* instancedGeo = dynamic_cast<const InstancedGeometry*>( &geometry );
    bool hasInstanceTransforms = false;
    if(instancedGeo)
    {
        _instanced = true;
        _instanceCount = instancedGeo->count();
        hasInstanceTransforms = instancedGeo->hasChanged() && instancedGeo->getTransforms().size()>0;
        std::cout << "instanced geometry detected" << std::endl;
        
        if(hasInstanceTransforms)
        {
            const std::vector<Matrix4>& bufMat = instancedGeo->getTransforms();
            _instanceCount = std::min(_instanceCount,(int)bufMat.size());
            
            std::vector<float> buf((size_t)(_instanceCount * 16));
            for(int i=0;i<_instanceCount;++i)
            {
                const float* data = bufMat[i].data();
                for(int k=0;k<16;++k) buf[(int)(i*16+k)] = data[k];
            }
            loadBuffer<float>(Buffer_InstTransforms, buf);
        }
    }
    

    GLuint glVertex = 0, glColor = 1, glNormal = 2, glTexCoord = 3;
    GLuint glInstTransform1 = 4, glInstTransform2 = 5, glInstTransform3 = 6, glInstTransform4 = 7;

    enableAttrib(Buffer_Vertices, glVertex, 3, sizeof(float)*3, 0);
    if(geometry._hasColors)
        enableAttrib(Buffer_Colors, glColor, 3, sizeof(float)*3, 0);
    if(geometry._hasNormals)
        enableAttrib(Buffer_Normals, glNormal, 3, sizeof(float)*3, 0);
    if(geometry._hasTexCoords)
        enableAttrib(Buffer_TexCoords, glTexCoord, 2, sizeof(float)*2, 0);
    if(_instanced && hasInstanceTransforms)
    {
        GLsizei vec4size = sizeof(float)*4;
        enableAttrib(Buffer_InstTransforms, glInstTransform1, 4, 4*vec4size, 0*4);
        enableAttrib(Buffer_InstTransforms, glInstTransform2, 4, 4*vec4size, 1*4);
        enableAttrib(Buffer_InstTransforms, glInstTransform3, 4, 4*vec4size, 2*4);
        enableAttrib(Buffer_InstTransforms, glInstTransform4, 4, 4*vec4size, 3*4);
        
        glVertexAttribDivisor(glInstTransform1, 1);
        glVertexAttribDivisor(glInstTransform2, 1);
        glVertexAttribDivisor(glInstTransform3, 1);
        glVertexAttribDivisor(glInstTransform4, 1);
    }
}

//--------------------------------------------------------------
void VertexArray::enableAttrib(BufferName bufName, GLuint location, int size, int stride, int offset)
{
    if(bufName == Buffer_Indices) return;

    GLenum glType = GL_FLOAT;
    void* addrOffset = (void*) (sizeof(float) * offset);
    use();
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[bufName]);
    GL_CHECKERROR("VertexArray::enableAttrib bind");
    glEnableVertexAttribArray(location);
    GL_CHECKERROR("VertexArray::enableAttrib enable");
    glVertexAttribPointer(location, size, glType, GL_FALSE, stride, addrOffset);
    GL_CHECKERROR("VertexArray::enableAttrib pointer");
}

//--------------------------------------------------------------
void VertexArray::use()
{
    glBindVertexArray(_vao);
    GL_CHECKERROR("VertexArray use");
}

//--------------------------------------------------------------
void VertexArray::draw()
{
    use();

    GLenum glPrimitive =  GL_TRIANGLES;
    if(_primitive == Geometry::Primitive_Points) glPrimitive = GL_POINTS;
    else if(_primitive == Geometry::Primitive_Lines) glPrimitive = GL_LINES;
    else if(_primitive == Geometry::Primitive_Triangles) glPrimitive = GL_TRIANGLES;

    if(_useElements && _instanced)
        glDrawElementsInstanced(glPrimitive,_count,GL_UNSIGNED_INT,(void*)0,_instanceCount);
    else if(_useElements)
        glDrawElements(glPrimitive, _count, GL_UNSIGNED_INT, (void*)0);
    else if(_instanced)
        glDrawArraysInstanced(glPrimitive,0,_count,_instanceCount);
    else
        glDrawArrays(glPrimitive, 0, _count);

    GL_CHECKERROR("VertexArray draw");
}


std::uint32_t VertexArray::s_count()
{
    return s_vertexArrayCount;
}

std::uint32_t VertexArray::s_vboCount()
{
    return s_vertexBufferObjectCount;
}


IMPGEARS_END
