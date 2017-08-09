#ifndef IMP_SHADOWBUFFERSHADER_H
#define IMP_SHADOWBUFFERSHADER_H

#include "Core/impBase.h"

#include <SceneGraph/Shader.h>

IMPGEARS_BEGIN

class IMP_API ShadowBufferShader : public Shader
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
