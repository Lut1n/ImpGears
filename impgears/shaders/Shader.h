#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include "base/impBase.hpp"
#include "base/Matrix4.h"
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
        void setMatrix4Parameter(const char* name, const Matrix4& Matrix4);
        void setVector3ArrayParameter(const char* name, float* vector3Array, Uint32 count);
        void setVector3Parameter(const char* name, const Vector3& vec3);

        void setProjection(const Matrix4& projection);
        void setView(const Matrix4& view);
        void setModel(const Matrix4& model);

        void enable();
        void disable();

        static const Shader* getActiveShader() {return m_instance;}

    protected:
    private:

        Uint32 m_vertexID;
        Uint32 m_fragmentID;
        Uint32 m_programID;

        static Shader* m_instance;
};

IMPGEARS_END

#endif // IMP_SHADER_H
