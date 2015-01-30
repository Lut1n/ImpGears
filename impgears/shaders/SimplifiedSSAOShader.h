#ifndef IMP_SIMPLIFIEDSSAOSHADER_H
#define IMP_SIMPLIFIEDSSAOSHADER_H

#include "base/impBase.hpp"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

class SimplifiedSSAOShader : public Shader
{
    public:
        SimplifiedSSAOShader();
        virtual ~SimplifiedSSAOShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_SIMPLIFIEDSSAOSHADER_H
