#ifndef DEFAULTSHADER_H
#define DEFAULTSHADER_H

#include <iostream>

#include "Shader.h"

IMPGEARS_BEGIN

class DefaultShader : public Shader
{
    public:
        DefaultShader();
        virtual ~DefaultShader();

    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // DEFAULTSHADER_H
