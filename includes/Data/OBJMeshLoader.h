#ifndef OBJMESHLOADER_H
#define OBJMESHLOADER_H

#include "Core/impBase.h"
#include "Data/MeshModel.h"

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
