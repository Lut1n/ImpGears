#include <SceneGraph/MeshNode.h>
#include <SceneGraph/DeferredShader.h>
#include <SceneGraph/ShadowBufferShader.h>

IMPGEARS_BEGIN

MeshNode::MeshNode(MeshModel* model):
    m_model(model)
{
}

MeshNode::~MeshNode()
{
}

void MeshNode::render()
{
    m_model->render();
}

IMPGEARS_END
