#ifndef IMP_BLINNPHONGSHADER_H
#define IMP_BLINNPHONGSHADER_H

#include "Shader.h"

IMPGEARS_BEGIN


class IMP_API BlinnPhongShader : public Shader
{
    public:
        BlinnPhongShader();
        virtual ~BlinnPhongShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_BLINNPHONGSHADER_H
