#ifndef IMP_RENDERPARAMETERS_H
#define IMP_RENDERPARAMETERS_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>

#include <SceneGraph/OpenGL.h>

IMPGEARS_BEGIN

/// \brief Defines the render parameters.
class IMP_API RenderParameters : public Object
{
    public:

        enum FaceCullingMode
        {
            FaceCullingMode_None = 0,
            FaceCullingMode_Front,
            FaceCullingMode_Back
        };

        enum BlendMode
        {
            BlendMode_None = 0,
            BlendMode_SrcAlphaBased
        };

        enum ParamState
        {
            ParamState_Disable = 0,
            ParamState_Enable
        };
		
		Meta_Class(RenderParameters)

        RenderParameters();
        RenderParameters(const RenderParameters& other);
        virtual ~RenderParameters();

        const RenderParameters& operator=(const RenderParameters& other);

        void setClearColor(const Vec3& clearColor);
        const Vec3& getClearColor() const{return m_clearColor;}

        void setFaceCullingMode(FaceCullingMode faceCullingMode){m_faceCullingMode = faceCullingMode;_faceCullingChanged=true;}
        FaceCullingMode getFaceCullingMode() const{return m_faceCullingMode;}

        void setBlendMode(BlendMode blendMode){m_blendMode = blendMode;_blendModeChanged=true;}
        BlendMode getBlendMode() const{return m_blendMode;}
		
		void apply(bool clearBuffer) const;

        void setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue);
        void setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue);

        void setProjectionMatrix(const Matrix4& projection){m_projectionMatrix = projection;}
        const Matrix4& getProjectionMatrix() const {return m_projectionMatrix;}
		
		void setFog(ParamState state);
		void setClearDepth(float depth);
		
		void setViewport(float x, float y, float width, float height);
        
        void setLineWidth(float lw);

    protected:
    private:

        Vec3 m_clearColor;
		float _clearDepth;
        Matrix4 m_projectionMatrix;
        FaceCullingMode m_faceCullingMode;
        BlendMode m_blendMode;
		ParamState _fogState;
        float _lineWidth;
		
		float _viewport[4];
		
		bool _clearColorChanged;
		bool _clearDepthChanged;
		bool _faceCullingChanged;
		bool _blendModeChanged;
		bool _fogChanged;
		bool _viewportChanged;
        bool _lineWidthChanged;
};

IMPGEARS_END

#endif // IMP_RENDERPARAMETERS_H
