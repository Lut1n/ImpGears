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
void FxRenderPass::setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output)
{
    _input = input;
    _output = output;
}

//--------------------------------------------------------------
Graph::Ptr FxRenderPass::buildQuadGraph(const std::string& debug_name,
                                        const std::string& glsl_lighting,
                                        Vec4 viewport) const
{
    ReflexionModel::Ptr shader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi,
                debug_name);
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

//--------------------------------------------------------------
void FxRenderPass::setInput(ImageSampler::Ptr sampler, int id)
{
    if(id >= (int)_input.size()) _input.resize(id+1);
    _input[id] = sampler;
}

//--------------------------------------------------------------
void FxRenderPass::setOutput(ImageSampler::Ptr sampler, int id)
{
    if(id >= (int)_output.size()) _output.resize(id+1);
    _output[id] = sampler;
}

IMPGEARS_END
