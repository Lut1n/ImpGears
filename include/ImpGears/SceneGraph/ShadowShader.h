#ifndef IMP_SHADOWSHADER_H
#define IMP_SHADOWSHADER_H

#include "Core/impBase.h"

#include <SceneGraph/Shader.h>

IMPGEARS_BEGIN

/// \brief Defines a shadow shader.
class IMP_API ShadowShader : public Shader
{
    public:
        ShadowShader();
        virtual ~ShadowShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

// source : http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

IMPGEARS_END

#endif // IMP_SHADOWSHADER_H
