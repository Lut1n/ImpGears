#ifndef GUI_SHADER_H
#define GUI_SHADER_H

#include <iostream>

#include <SceneGraph/Shader.h>

IMPGEARS_BEGIN

/// \brief Defines the default shader.
class IMP_API GuiShader : public Shader
{
    public:
        GuiShader();
        virtual ~GuiShader();

    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // GUI_SHADER_H
