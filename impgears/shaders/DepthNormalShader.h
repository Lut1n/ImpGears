#ifndef IMP_DEPTHNORMALSHADER_H
#define IMP_DEPTHNORMALSHADER_H

#include "base/impBase.hpp"

#include "shaders/Shader.h"

IMPGEARS_BEGIN

/// \brief Defines the depth and normals rendering shader.
class DepthNormalShader : public Shader
{
    public:
        DepthNormalShader();
        virtual ~DepthNormalShader();
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_DEPTHNORMALSHADER_H
