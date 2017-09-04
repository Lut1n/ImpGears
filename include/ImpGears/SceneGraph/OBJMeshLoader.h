#ifndef OBJMESHLOADER_H
#define OBJMESHLOADER_H

#include "Core/impBase.h"
#include <SceneGraph/Mesh.h>

IMPGEARS_BEGIN

class IMP_API OBJMeshLoader
{
    public:
        OBJMeshLoader();
        virtual ~OBJMeshLoader();

        static Mesh* loadFromFile(const char* filename);

		static bool saveToFile(const Mesh* mesh, const char* filename);

    protected:

    private:
};

IMPGEARS_END

#endif // OBJMESHLOADER_H
