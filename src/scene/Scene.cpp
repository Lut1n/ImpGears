#include "scene/Scene.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Scene* Scene::instance = NULL;

//--------------------------------------------------------------
Scene::Scene()
{
    Scene::setInstance(this);
}

//--------------------------------------------------------------
Scene::~Scene(){
}

//--------------------------------------------------------------
void Scene::render(imp::Uint32 passID){
}

//--------------------------------------------------------------
void Scene::addSceneComponent(SceneNode* sceneNode){
    addSubNode(sceneNode);
}

//--------------------------------------------------------------
void Scene::removeSceneComponent(SceneNode* sceneNode){
    removeSubNode(sceneNode);
}

IMPGEARS_END
