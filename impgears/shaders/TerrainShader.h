#ifndef IMP_TERRAINSHADER_H
#define IMP_TERRAINSHADER_H

#include "base/impBase.hpp"

#include "shaders/Shader.h"

IMPGEARS_BEGIN

/// \brief Defines a shader for the voxel terrain.
class TerrainShader : public Shader
{
    public:
        TerrainShader();
        virtual ~TerrainShader();

        static TerrainShader* instance;
    protected:
    private:

        static const char* vertexCodeSource;
        static const char* fragmentCodeSource;
};

IMPGEARS_END

#endif // IMP_TERRAINSHADER_H
