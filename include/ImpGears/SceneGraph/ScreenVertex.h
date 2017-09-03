#ifndef IMP_SCREENVERTEX_H
#define IMP_SCREENVERTEX_H

#include "Core/impBase.h"
#include <SceneGraph/VBOData.h>
#include <SceneGraph/SceneNode.h>

IMPGEARS_BEGIN

class IMP_API ScreenVertex : public VBOData, public SceneNode
{
    public:
        ScreenVertex();
        virtual ~ScreenVertex();

        virtual void render();

    protected:
    private:

        Uint64 m_texCoordOffset;
};

IMPGEARS_END

#endif // IMP_SCREENVERTEX_H
