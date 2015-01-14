#include "Scene.h"

Scene* Scene::instance = NULL;

Scene::Scene():
    sky(NULL)
{
    //addSubSceneNode(&world);
    addSubSceneNode(&terrain);
    Scene::setInstance(this);
}

Scene::~Scene(){
}

void Scene::render(){
    if(sky != NULL)sky->render();
}

void Scene::update(){
    if(sky != NULL){
            imp::Vector3 pos = imp::Camera::getActiveCamera()->getPosition();
            sky->update(pos, pos);
    }
}

void Scene::addSceneComponent(SceneNode* SceneNode){
    addSubSceneNode(SceneNode);
}

void Scene::removeSceneComponent(SceneNode* SceneNode){
    removeSubSceneNode(SceneNode);
}

void Scene::onEvent(imp::Event evn){}
