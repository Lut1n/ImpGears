#ifndef IMP_WATER_H
#define IMP_WATER_H

#include "base/impBase.h"

#include "Entity.h"
#include "scene/SceneNode.h"
#include "graphics/VBOData.h"

IMPGEARS_BEGIN

class Water : public Entity, public SceneNode, public VBOData
{
    public:
        Water(const Vector3& tileVolume);
        virtual ~Water();

        virtual void render(Uint32 passID);
        virtual void update();
        virtual void onEvent(const Event& event);

    protected:
    private:

        Vector3 m_tileVolume;
};

IMPGEARS_END

#endif // IMP_WATER_H
