#ifndef GRAPHICRENDERER_H
#define GRAPHICRENDERER_H

#include "base/impBase.hpp"
#include "EvnContextInterface.h"
#include "Scene.h"
#include "camera/SceneCamera.h"

IMPGEARS_BEGIN

class GraphicRenderer
{
    public:
        GraphicRenderer(Uint32 windowID, Camera* camera);
        virtual ~GraphicRenderer();

        void renderScene();

        void updateScene();

        void onEventScene(imp::Event evn);

        Camera* getCamera(){return camera;}

        static GraphicRenderer* getInstance(){return instance;}

        void setCenterCursor(bool center){centerCursor = center;}

        Scene* getScene(){return &scene;}

    protected:
        Uint32 windowID;
        Scene scene;
        Camera* camera;

        static void setInstance(GraphicRenderer* inst){instance = inst;}

    private:

        static GraphicRenderer* instance;
        bool centerCursor;
};

IMPGEARS_END

#endif // GRAPHICRENDERER_H
