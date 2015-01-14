#ifndef Scene_H
#define Scene_H

#include "SceneNode.h"
//#include "lights/LightManager.h"
#include "terrain/TerrainNode.h"
#include "sky/Sky.h"

class Scene : public SceneNode
{
    public:
        Scene();
        virtual ~Scene();

        virtual void render();
        virtual void update();
        virtual void onEvent(imp::Event evn);

        void addSceneComponent(SceneNode* SceneNode);
        void removeSceneComponent(SceneNode* SceneNode);

        void setSky(Sky* sky){this->sky = sky;}

        TerrainSceneNode* getTerrain(){return &terrain;}

        static Scene* getInstance(){return instance;}
    protected:
        TerrainSceneNode terrain;

        static void setInstance(Scene* i){instance = i;}
    private:
//        LightManager Lights;
        Sky* sky;

        static Scene* instance;
};

#endif // Scene_H
