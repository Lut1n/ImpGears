#ifndef OBJMESHLOADER_H
#define OBJMESHLOADER_H

#include "base/impBase.h"
#include "mesh/MeshModel.h"

IMPGEARS_BEGIN

class IMP_API OBJMeshLoader
{
    public:
        OBJMeshLoader();
        virtual ~OBJMeshLoader();

        static MeshModel* loadFromFile(const char* filename);

    protected:

    private:
};

IMPGEARS_END

#endif // OBJMESHLOADER_H
