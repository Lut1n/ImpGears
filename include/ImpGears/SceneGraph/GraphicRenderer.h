#ifndef IMP_GRAPHICRENDERER_H
#define IMP_GRAPHICRENDERER_H

#include <Core/Object.h>
#include <Core/Matrix4.h>

#include <SceneGraph/SceneNode.h>
#include <SceneGraph/RenderParameters.h>

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class IMP_API GraphicRenderer : public Object
{
    public:
	
		Meta_Class(GraphicRenderer)
		
        GraphicRenderer();
        virtual ~GraphicRenderer();

        void renderScene();

        void setRenderParameters(RenderParameters::Ptr& parameters){_parameters = parameters; _state->setParameters(parameters);}
		RenderParameters::Ptr getRenderParameters() const {return _parameters;}


        static GraphicRenderer* getInstance(){return instance;}

        void setCenterCursor(bool center){centerCursor = center;}

		void setSceneRoot(const SceneNode::Ptr& root){_root = root; }
        SceneNode* getSceneRoot(){return _root.get();}

        //void setProjectionMatrix(const Matrix4& projMat) {m_projMat = projMat;}
        const Matrix4& getProjectionMatrix() const {return _parameters->getProjectionMatrix();}
		
		GraphicStatesManager& getStateManager() {return _stateMgr;}

    protected:

        GraphicState::Ptr _state;
		RenderParameters::Ptr _parameters;
        SceneNode::Ptr _root;

        static void setInstance(GraphicRenderer* inst){instance = inst;}

    private:

        static GraphicRenderer* instance;
        bool centerCursor;

		GraphicStatesManager _stateMgr;
        //Matrix4 m_projMat;
};

IMPGEARS_END

#endif // IMP_GRAPHICRENDERER_H
