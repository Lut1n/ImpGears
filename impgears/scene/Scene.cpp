#include "Scene.h"

Scene* Scene::instance = NULL;

Scene::Scene()
{
    Scene::setInstance(this);
}

Scene::~Scene(){
}

void Scene::render(imp::Uint32 passID){
}

void Scene::addSceneComponent(SceneNode* SceneNode){
    addSubNode(SceneNode);
}

void Scene::removeSceneComponent(SceneNode* SceneNode){
    removeSubNode(SceneNode);
}
