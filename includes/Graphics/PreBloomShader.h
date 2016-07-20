#ifndef PREBLOOMSHADER_H
#define PREBLOOMSHADER_H

#include <iostream>

#include "Shader.h"

IMPGEARS_BEGIN

class IMP_API PreBloomShader : public Shader
{
    public:
        PreBloomShader();
        virtual ~PreBloomShader();

    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // PREBLOOMSHADER_H
