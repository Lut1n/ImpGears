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

        virtual void render(imp::Uint32 passID);

        void addSceneComponent(SceneNode* SceneNode);
        void removeSceneComponent(SceneNode* SceneNode);

        static Scene* getInstance(){return instance;}
    protected:

        static void setInstance(Scene* i){instance = i;}
    private:

        static Scene* instance;
};

#endif // Scene_H
