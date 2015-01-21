#ifndef IMP_SSAOSHADER_H
#define IMP_SSAOSHADER_H

#include "base/impBase.hpp"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

class SSAOShader : public Shader
{
    public:
        SSAOShader();
        virtual ~SSAOShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_SSAOSHADER_H
