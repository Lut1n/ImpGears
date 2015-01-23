#ifndef IMP_SSAOSHADER_H
#define IMP_SSAOSHADER_H

#include "base/impBase.hpp"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

/// \brief Defines the Screen Space Ambient Occlusion shader.
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
