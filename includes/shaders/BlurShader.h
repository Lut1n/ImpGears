#ifndef IMP_BLURSHADER_H
#define IMP_BLURSHADER_H

#include "shaders/Shader.h"

IMPGEARS_BEGIN

class IMP_API BlurShader : public Shader
{
    public:
        BlurShader();
        virtual ~BlurShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_BLURSHADER_H
