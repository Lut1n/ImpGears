#ifndef IMP_DEBUGTEXTURESHADER_H
#define IMP_DEBUGTEXTURESHADER_H

#include "base/impBase.hpp"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

class DebugTextureShader : public Shader
{
    public:
        DebugTextureShader();
        virtual ~DebugTextureShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_DEBUGTEXTURESHADER_H
