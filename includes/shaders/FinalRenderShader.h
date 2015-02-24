#ifndef IMP_FINALRENDERSHADER_H
#define IMP_FINALRENDERSHADER_H

#include "base/impBase.h"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

class IMP_API FinalRenderShader : public Shader
{
    public:
        FinalRenderShader();
        virtual ~FinalRenderShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_FINALRENDERSHADER_H
