#ifndef IMP_SSAOSHADER_H
#define IMP_SSAOSHADER_H

#include "base/impBase.hpp"
#include "shaders/Shader.h"

IMPGEARS_BEGIN

#define IMP_SSAO_SAMPLE_KERNEL_SIZE 16

/// \brief Defines the Screen Space Ambient Occlusion shader.
class SSAOShader : public Shader
{
    public:
        SSAOShader();
        virtual ~SSAOShader();

        const Texture* getNoiseTexture() const{return &m_noise;}
        float* getSampleKernel() {return m_sampleKernel;}

        static SSAOShader* instance;
    protected:
    private:

        float m_sampleKernel[IMP_SSAO_SAMPLE_KERNEL_SIZE*3];

        Texture m_noise;

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

// source : http://john-chapman-graphics.blogspot.fr/2013/01/ssao-tutorial.html

IMPGEARS_END

#endif // IMP_SSAOSHADER_H
