#ifndef IMP_LIGHTING_MODEL_H
#define IMP_LIGHTING_MODEL_H

#include <OGLPlugin/Pipeline.h>
#include <OGLPlugin/Export.h>

IMPGEARS_BEGIN

class GlRenderer;

class IG_BKND_GL_API LightingModel : public FrameOperation
{
public:
    Meta_Class(LightingModel)

    LightingModel();
    virtual ~LightingModel();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

    void setLightPower(float lightpower) {_lightpower=lightpower;}
    void setAmbient(float ambient) {_ambient=ambient;}

protected:
    Graph::Ptr _graph;
    Graph::Ptr _fillingGraph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
    RenderQueue::Ptr _fillingQueue;

    float _lightpower;
    float _ambient;
};

IMPGEARS_END

#endif // IMP_LIGHTING_MODEL_H
