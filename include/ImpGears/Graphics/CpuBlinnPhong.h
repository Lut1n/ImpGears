#ifndef IMP_CPU_BLINN_PHONG_H
#define IMP_CPU_BLINN_PHONG_H

#include <Graphics/Rasterizer.h>

IMPGEARS_BEGIN

struct IMP_API CpuBlinnPhong : public FragCallback
{
	
	Meta_Class(CpuBlinnPhong)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr);
};

IMPGEARS_END

#endif // IMP_CPU_BLINN_PHONG_H
