#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include "base/impBase.hpp"
#include "base/Mat4.h"
#include "graphics/Texture.h"

IMPGEARS_BEGIN

#define IMP_GLSL_SRC( src ) #src

/// \brief Defines a shader (vertex and fragment shaders).
class Shader
{
    public:
        Shader(const char* vertexShader, const char* fragmentShader);
        virtual ~Shader();

        void setTextureParameter(const char* name, const Texture* texture, Int32 textureUnit = 0);
        void setFloatParameter(const char* name, float value);
        void setMat4Parameter(const char* name, const Mat4& mat4);

        void enable();
        void disable();
    protected:
    private:

        Uint32 m_vertexID;
        Uint32 m_fragmentID;
        Uint32 m_programID;
};

IMPGEARS_END

#endif // IMP_SHADER_H
