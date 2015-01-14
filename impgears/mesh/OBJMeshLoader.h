#ifndef OBJMESHLOADER_H
#define OBJMESHLOADER_H

#include "base/impBase.hpp"
#include "mesh/MeshModel.h"

IMPGEARS_BEGIN

class OBJMeshLoader
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
