#ifndef IMP_CPU_BLINN_PHONG_H
#define IMP_CPU_BLINN_PHONG_H

#include <ImpGears/Graphics/Rasterizer.h>

IMPGEARS_BEGIN

struct IMP_API CpuBlinnPhong : public FragCallback
{

    Meta_Class(CpuBlinnPhong)
    virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings);
};

IMPGEARS_END

#endif // IMP_CPU_BLINN_PHONG_H
