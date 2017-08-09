#ifndef IMP_BLOOMSHADER_H
#define IMP_BLOOMSHADER_H

#include "Graphics/Shader.h"

IMPGEARS_BEGIN

class IMP_API BloomShader : public Shader
{
    public:
        BloomShader();
        virtual ~BloomShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_BLOOMSHADER_H
