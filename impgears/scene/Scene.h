#ifndef Scene_H
#define Scene_H

#include "SceneNode.h"
//#include "lights/LightManager.h"
#include "terrain/TerrainNode.h"
#include "sky/Sky.h"

/// \brief Defines the Scene. A scene is rendered by the graphic renderer.
class Scene : public SceneNode
{
    public:
        Scene();
        virtual ~Scene();

        virtual void render(imp::Uint32 passID);

        /// \brief Adds a node to the scene.
        /// \param sceneNode - the node to add.
        void addSceneComponent(SceneNode* sceneNode);

        /// \brief Removes a node from the scene.
        /// \param sceneNode - the node to remove.
        void removeSceneComponent(SceneNode* sceneNode);

        static Scene* getInstance(){return instance;}
    protected:

        static void setInstance(Scene* i){instance = i;}
    private:

        static Scene* instance;
};

#endif // Scene_H
