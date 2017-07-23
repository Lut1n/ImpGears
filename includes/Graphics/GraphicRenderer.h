#ifndef IMP_GRAPHICRENDERER_H
#define IMP_GRAPHICRENDERER_H

#include "Core/impBase.h"
#include "Math/Matrix4.h"
#include "System/EvnContextInterface.h"
#include "Graphics/SceneNode.h"
#include "Graphics/Camera.h"
#include "Graphics/RenderParameters.h"

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class IMP_API GraphicRenderer
{
    public:
        GraphicRenderer(Uint32 windowID, Camera* camera);
        virtual ~GraphicRenderer();

        void renderScene(Uint32 passID);

        void setCamera(Camera* camera){this->camera = camera; if(camera != IMP_NULL)camera->activate();}
        Camera* getCamera(){return camera;}

        void setRenderParameters(const RenderParameters& parameters){m_parameters = parameters;}
        const RenderParameters& getRenderParameters() const {return m_parameters;}

        static GraphicRenderer* getInstance(){return instance;}

        void setCenterCursor(bool center){centerCursor = center;}

		void setSceneRoot(const std::shared_ptr<SceneNode>& root){_root = root; }
        SceneNode* getSceneRoot(){return _root.get();}

        //void setProjectionMatrix(const Matrix4& projMat) {m_projMat = projMat;}
        const Matrix4& getProjectionMatrix() const {return m_parameters.getProjectionMatrix();}
		
		GraphicStatesManager& getStateManager() {return _stateMgr;}

    protected:

        RenderParameters m_parameters;
        Uint32 windowID;
        std::shared_ptr<SceneNode> _root;
        Camera* camera;

        static void setInstance(GraphicRenderer* inst){instance = inst;}

    private:

        static GraphicRenderer* instance;
        bool centerCursor;

		GraphicStatesManager _stateMgr;
        //Matrix4 m_projMat;
};

IMPGEARS_END

#endif // IMP_GRAPHICRENDERER_H
