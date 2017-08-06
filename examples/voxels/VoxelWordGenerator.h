/*
Data/IGWorldGenerator.h

Define a generator for world creating.

Two main functions can be used.
- The first one generates a world with random height at each XY position.
- The second one generates a world with a texture as heightmap. The scale of the texture is automatically adapted to the world dimensions.
*/

#ifndef IMP_WORDGENERATOR_H
#define IMP_WORDGENERATOR_H

#include <Core/impBase.h>
#include <Graphics/ImageData.h>

#include "VoxelWorld.h"

IMPGEARS_BEGIN

class IMP_API VoxelWordGenerator
{
private:

    VoxelWordGenerator();

    static VoxelWordGenerator* instance;

public:

    static VoxelWordGenerator* GetInstance();

    ~VoxelWordGenerator();

	// Generate random world
    void Generate(VoxelWorld* _world);

	// Generate world from heightmap
    void Generate(VoxelWorld* _world, const imp::ImageData* _heightmap);

    void InitRand();

    imp::Uint32 Rand(imp::Uint32 _min, imp::Uint32 _max);
};

IMPGEARS_END

#endif // IMP_WORDGENERATOR_H
