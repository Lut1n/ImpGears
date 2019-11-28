#include <OGLPlugin/FxRenderPass.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
FxRenderPass::FxRenderPass()
{

}

//--------------------------------------------------------------
FxRenderPass::~FxRenderPass()
{

}

//--------------------------------------------------------------
Graph::Ptr FxRenderPass::buildQuadGraph(const std::string& glsl_lighting, Vec4 viewport) const
{
    ReflexionModel::Ptr shader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi);
    shader->_fragCode_lighting = glsl_lighting;

    QuadNode::Ptr quad = QuadNode::create();
    quad->setReflexion(shader);
    Node::Ptr root = quad;

    Graph::Ptr graph = Graph::create();
    graph->setRoot(root);
    graph->getInitState()->setViewport( viewport );
    graph->getInitState()->setOrthographicProjection(-1.0,1.0,-1.0,1.0,0.0,1.0);

    return graph;
}

IMPGEARS_END
