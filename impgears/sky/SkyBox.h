#ifndef SKYBOX_H
#define SKYBOX_H

#include "Sky.h"
#include "../graphics/TextureManager.h"

#define SKYBOX_SIZE 5000.f

typedef struct{

    float top_x, top_y;
    float bottom_x, bottom_y;
    float front_x, front_y;
    float back_x, back_y;
    float left_x, left_y;
    float right_x, right_y;
    float width, height;
    bool isBottom;

} SkyBoxProperties;

class SkyBox : public Sky
{
    public:
        SkyBox();
        SkyBox(const std::string& filename, const SkyBoxProperties properties);
        virtual ~SkyBox();

        virtual void load(const std::string& filename);
        virtual void update(const imp::Vector3& position, const imp::Vector3& orientation);
        virtual void render();

        static SkyBoxProperties getCrossSkyBoxProperties();
    protected:
    private:
        imp::Texture* texture;
        imp::Vector3 position;
        SkyBoxProperties properties;
};

#endif // SKYBOX_H
