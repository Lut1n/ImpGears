#ifndef IMP_GRAPHICRENDERER_H
#define IMP_GRAPHICRENDERER_H

#include "Core/impBase.h"
#include <Core/Matrix4.h>

#include <SceneGraph/SceneNode.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/RenderParameters.h>

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class IMP_API GraphicRenderer
{
    public:
        GraphicRenderer();
        virtual ~GraphicRenderer();

        void renderScene();

        void setRenderParameters(std::shared_ptr<RenderParameters>& parameters){_parameters = parameters;}
		std::shared_ptr<RenderParameters> getRenderParameters() const {return _parameters;}


        static GraphicRenderer* getInstance(){return instance;}

        void setCenterCursor(bool center){centerCursor = center;}

		void setSceneRoot(const std::shared_ptr<SceneNode>& root){_root = root; }
        SceneNode* getSceneRoot(){return _root.get();}

        //void setProjectionMatrix(const Matrix4& projMat) {m_projMat = projMat;}
        const Matrix4& getProjectionMatrix() const {return _parameters->getProjectionMatrix();}
		
		GraphicStatesManager& getStateManager() {return _stateMgr;}

    protected:

        std::shared_ptr<GraphicState> _state;
		std::shared_ptr<RenderParameters> _parameters;
        std::shared_ptr<SceneNode> _root;

        static void setInstance(GraphicRenderer* inst){instance = inst;}

    private:

        static GraphicRenderer* instance;
        bool centerCursor;

		GraphicStatesManager _stateMgr;
        //Matrix4 m_projMat;
};

IMPGEARS_END

#endif // IMP_GRAPHICRENDERER_H
