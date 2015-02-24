#include "mesh/MeshNode.h"
#include "shaders/DeferredShader.h"

IMPGEARS_BEGIN

MeshNode::MeshNode(MeshModel* model):
    m_model(model)
{
}

MeshNode::~MeshNode()
{
}

void MeshNode::render(imp::Uint32 passID)
{
    if(passID == 3)
    {
        imp::DeferredShader::instance->setMatrix4Parameter("u_model", getModelMatrix());
        imp::DeferredShader::instance->setMatrix4Parameter("u_normal", getModelMatrix());
        imp::DeferredShader::instance->setMatrix4Parameter("u_shadowModelMat", getModelMatrix());
    }
    m_model->render();
}

IMPGEARS_END
