#ifndef IMP_SHADOWBUFFERSHADER_H
#define IMP_SHADOWBUFFERSHADER_H

#include "base/impBase.hpp"

#include "shaders/Shader.h"

IMPGEARS_BEGIN

class ShadowBufferShader : public Shader
{
    public:
        ShadowBufferShader();
        virtual ~ShadowBufferShader();

        static ShadowBufferShader* instance;
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_SHADOWBUFFERSHADER_H
