#ifndef BASICSHADER_H
#define BASICSHADER_H

#include <iostream>

#include "Shader.h"

IMPGEARS_BEGIN

/// \brief Defines the default shader.
class IMP_API BasicShader : public Shader
{
    public:
        BasicShader();
        virtual ~BasicShader();

    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // BASICSHADER_H
