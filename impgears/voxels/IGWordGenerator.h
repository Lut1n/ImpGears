/*
voxels/IGWorldGenerator.h

Define a generator for world creating.

Two main functions can be used.
- The first one generates a world with random height at each XY position.
- The second one generates a world with a texture as heightmap. The scale of the texture is automatically adapted to the world dimensions.
*/

#ifndef IGWORDGENERATOR_H
#define IGWORDGENERATOR_H

#include "../base/impBase.hpp"
#include "../voxels/IGWorld.h"
#include "../graphics/Texture.h"

class IGWordGenerator
{
private:

    IGWordGenerator();

    static IGWordGenerator* instance;

public:

    static IGWordGenerator* GetInstance();

    ~IGWordGenerator();

	// Generate random world
    void Generate(IGWorld* _world);

	// Generate world from heightmap
    void Generate(IGWorld* _world, const imp::Texture* _heightmap);

    void InitRand();

    imp::Uint32 Rand(imp::Uint32 _min, imp::Uint32 _max);
};

#endif // IGWORDGENERATOR_H
