#ifndef IMP_RENDER_PASS_H
#define IMP_RENDER_PASS_H

#include <ImpGears/Graphics/Rasterizer.h>
#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/SceneGraph/Export.h>

IMPGEARS_BEGIN

struct IG_SCENE_API RenderPass : public Object
{
    Meta_Class(RenderPass);

    enum PassFlag
    {
        Pass_None                   = 0,
        Pass_DefaultMRT             = 1,
        Pass_EnvironmentMapping     = 1 << 1,
        Pass_ShadowMapping          = 1 << 2,
    };

    enum PostFXFlag
    {
        FX_None                     = 0,
        FX_AmbientOcclusion         = 1,
        FX_Bloom                    = 1 << 1
    };

    RenderPass(PassFlag rp = Pass_DefaultMRT, PostFXFlag fx = FX_None);
    virtual ~RenderPass();

    void setPassFlags(PassFlag rp);
    PassFlag getPassFlags() const;

    void setPostFXFlags(PostFXFlag fx);
    PostFXFlag getPostFXFlags() const;

    void enablePass( PassFlag rp );
    void disablePass( PassFlag rp );

    void enableFX( PostFXFlag fx );
    void disableFX( PostFXFlag fx );

    bool isPassEnabled(PassFlag rp) const;
    bool isFXEnabled(PostFXFlag fx) const;

    PassFlag _passFlags;
    PostFXFlag _postFXFlags;
};

IMPGEARS_END

#endif // IMP_RENDER_PASS_H
