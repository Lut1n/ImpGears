#ifndef IMP_SKYBOX_H
#define IMP_SKYBOX_H

#include "base/impBase.hpp"
#include "Entity.h"
#include "scene/SceneNode.h"
#include "graphics/VBOData.h"
#include "graphics/Texture.h"
#include "shaders/SkyShader.h"

IMPGEARS_BEGIN

/// \brief Defines a sky box.
class SkyBox : public Entity, public SceneNode, public VBOData
{
    public:
        SkyBox(const Texture* left, const Texture* right, const Texture* front, const Texture* back, const Texture* top, const Texture* bottom, float scale);
        virtual ~SkyBox();

        virtual void render(Uint32 passID);
        virtual void update();
        virtual void onEvent(const Event& evn);

    protected:
    private:

        Uint64 m_leftOffset, m_rightOffset, m_frontOffset, m_backOffset, m_topOffset, m_bottomOffset;
        Uint64 m_txCoordOffset;

        const Texture* m_left;
        const Texture* m_right;
        const Texture* m_front;
        const Texture* m_back;
        const Texture* m_top;
        const Texture* m_bottom;

        Vector3 m_center;
        float m_scale;

        SkyShader m_shader;
};

IMPGEARS_END

#endif // IMP_SKYBOX_H
