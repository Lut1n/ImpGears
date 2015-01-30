#ifndef GRAPHICRENDERER_H
#define GRAPHICRENDERER_H

#include "base/impBase.hpp"
#include "base/Mat4.h"
#include "EvnContextInterface.h"
#include "Scene.h"
#include "camera/SceneCamera.h"
#include "scene/RenderParameters.h"

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class GraphicRenderer
{
    public:
        GraphicRenderer(Uint32 windowID, Camera* camera);
        virtual ~GraphicRenderer();

        void renderScene(Uint32 passID);

        void setCamera(Camera* camera){this->camera = camera; if(camera != IMP_NULL)camera->activate();}
        Camera* getCamera(){return camera;}

        void setRenderParameters(const RenderParameters& parameters){m_parameters = parameters;}

        static GraphicRenderer* getInstance(){return instance;}

        void setCenterCursor(bool center){centerCursor = center;}

        Scene* getScene(){return &scene;}

        void setPerspectiveProjection();
        void setOrthographicProjection(float left, float right, float bottom, float top);

        const Mat4 getProjectionMatrix() const;
        const Mat4 getModelViewMatrix() const;

        const Mat4& getViewMatrix() const {return m_viewMat;}

    protected:

        RenderParameters m_parameters;
        Uint32 windowID;
        Scene scene;
        Camera* camera;

        static void setInstance(GraphicRenderer* inst){instance = inst;}

    private:

        static GraphicRenderer* instance;
        bool centerCursor;

        Mat4 m_viewMat;
};

IMPGEARS_END

#endif // GRAPHICRENDERER_H
