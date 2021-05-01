#ifndef IMP_CPU_BLINN_PHONG_H
#define IMP_CPU_BLINN_PHONG_H

#include <ImpGears/Graphics/Rasterizer.h>
#include <ImpGears/Renderer/Export.h>

IMPGEARS_BEGIN

struct IG_RENDERER_API CpuBlinnPhong : public FragCallback
{

    Meta_Class(CpuBlinnPhong)
    virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings);
};

IMPGEARS_END

#endif // IMP_CPU_BLINN_PHONG_H
