#ifndef IMP_SHADER_H
#define IMP_SHADER_H

#include "../base/impBase.hpp"

IMPGEARS_BEGIN

class Shader
{
    public:
        Shader(const char* vertexShader, const char* fragmentShader);
        virtual ~Shader();

        void setParameterValue(const char* name, void* value);

        void enable();
        void disable();
    protected:
    private:

        Uint32 m_vertexID;
        Uint32 m_fragmentID;
        Uint32 m_programID;
};

IMPGEARS_END

#endif // IMP_SHADER_H
