#ifndef DEFAULTSHADER_H
#define DEFAULTSHADER_H

#include <iostream>

#include "Shader.h"

IMPGEARS_BEGIN

/// \brief Defines the default shader.
class IMP_API DefaultShader : public Shader
{
    public:
        DefaultShader();
        virtual ~DefaultShader();

		static DefaultShader* getInstance();
		
		static DefaultShader* _instance;
		
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // DEFAULTSHADER_H
