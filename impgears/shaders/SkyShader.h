#ifndef IMP_SKYSHADER_H
#define IMP_SKYSHADER_H

#include "base/impBase.hpp"

#include "shaders/Shader.h"

IMPGEARS_BEGIN

/// \brief Defines a shader to be used with the skybox.
class SkyShader : public Shader
{
    public:
        SkyShader();
        virtual ~SkyShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_SKYSHADER_H
