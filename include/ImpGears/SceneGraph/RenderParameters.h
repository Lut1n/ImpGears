#ifndef IMP_RENDERPARAMETERS_H
#define IMP_RENDERPARAMETERS_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>
#include <Core/Vec4.h>

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
		
		Meta_Class(RenderParameters)

        RenderParameters();
        RenderParameters(const RenderParameters& other);
        virtual ~RenderParameters();

        const RenderParameters& operator=(const RenderParameters& other);

        void setFaceCullingMode(FaceCullingMode faceCullingMode){_faceCullingMode = faceCullingMode;_faceCullingChanged=true;}
        FaceCullingMode getFaceCullingMode() const{return _faceCullingMode;}

        void setBlendMode(BlendMode blendMode){_blendMode = blendMode; _blendModeChanged=true;}
        BlendMode getBlendMode() const{return _blendMode;}
		
		void apply() const;

        void setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue);
        void setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue);

        void setProjectionMatrix(const Matrix4& projection){_projection = projection;}
        const Matrix4& getProjectionMatrix() const {return _projection;}
		
		void setDepthTest(bool depthTest);
		
		void setViewport(float x, float y, float width, float height);
        
        void setLineWidth(float lw);

    protected:
    private:

		Matrix4 _projection;
		Vec4 _viewport;
		FaceCullingMode _faceCullingMode;
		BlendMode _blendMode;
		float _lineWidth;
		bool _depthTest;
		
		bool _viewportChanged;
		bool _faceCullingChanged;
		bool _blendModeChanged;
        bool _lineWidthChanged;
		bool _depthTestChanged;
};

IMPGEARS_END

#endif // IMP_RENDERPARAMETERS_H
