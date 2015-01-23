#ifndef IMP_SHADOWSHADER_H
#define IMP_SHADOWSHADER_H

#include "base/impBase.hpp"

#include "shaders/Shader.h"

IMPGEARS_BEGIN

/// \brief Defines a shadow shader.
class ShadowShader : public Shader
{
    public:
        ShadowShader();
        virtual ~ShadowShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_SHADOWSHADER_H
