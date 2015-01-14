#include "SkyBox.h"
#include "../graphics/GLcommon.h"

SkyBox::SkyBox()
{
    //ctor
}

SkyBox::SkyBox(const std::string& filename, const SkyBoxProperties properties)// :
    //Sky(filename)
{
    this->properties = properties;
    load(filename);
}

SkyBox::~SkyBox()
{
    //dtor
}

void SkyBox::load(const std::string& filename){
    texture = imp::TextureManager::getTexture(filename);
}

void SkyBox::update(const imp::Vector3& position, const imp::Vector3& orientation){
    this->position = position;
}

void SkyBox::render(){

    glPushMatrix();

    glTranslatef(position.getX(), position.getY(), position.getZ());

    SkyBoxProperties* p = &properties;

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    texture->bind();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //left
    glBegin(GL_QUADS);
    glTexCoord2f(p->left_x, p->left_y);
    glVertex3f(-SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->left_x, p->left_y+p->height);
    glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->left_x+p->width, p->left_y+p->height);
    glVertex3f( -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->left_x+p->width, p->left_y);
    glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE, SKYBOX_SIZE);
    glEnd();

    //front
    glBegin(GL_QUADS);
    glTexCoord2f(p->front_x, p->front_y);
    glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->front_x, p->front_y+p->height);
    glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->front_x+p->width, p->front_y+p->height);
    glVertex3f( SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->front_x+p->width, p->front_y);
    glVertex3f( SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
    glEnd();

    //right
    glBegin(GL_QUADS);
    glTexCoord2f(p->right_x, p->right_y);
    glVertex3f(SKYBOX_SIZE,  SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->right_x, p->right_y+p->height);
    glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->right_x+p->width, p->right_y+p->height);
    glVertex3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->right_x+p->width, p->right_y);
    glVertex3f( SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE);
    glEnd();

    //back
    glBegin(GL_QUADS);
    glTexCoord2f(p->back_x, p->back_y);
    glVertex3f(SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->back_x, p->back_y+p->height);
    glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->back_x+p->width, p->back_y+p->height);
    glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
    glTexCoord2f(p->back_x+p->width, p->back_y);
    glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE);
    glEnd();

    //top
    glBegin(GL_QUADS);
    glTexCoord2f(p->top_x, p->top_y);
    glVertex3f(-SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->top_x, p->top_y+p->height);
    glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->top_x+p->width, p->top_y+p->height);
    glVertex3f( SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE);
    glTexCoord2f(p->top_x+p->width, p->top_y);
    glVertex3f( SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE);
    glEnd();

    if(p->isBottom){

        //bottom
        glBegin(GL_QUADS);
        glTexCoord2f(p->bottom_x, p->bottom_y);
        glVertex3f(-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE);
        glTexCoord2f(p->bottom_x, p->bottom_y+p->height);
        glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
        glTexCoord2f(p->bottom_x+p->width, p->bottom_y+p->height);
        glVertex3f( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
        glTexCoord2f(p->bottom_x+p->width, p->bottom_y);
        glVertex3f( SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE);
        glEnd();

    }

    glPopMatrix();
}


SkyBoxProperties SkyBox::getCrossSkyBoxProperties(){
    SkyBoxProperties properties;
    properties.left_x = 0.0f;
    properties.left_y = 1.f/3.f + 0.001f;
    properties.front_x = 0.25f;
    properties.front_y = 1.f/3.f + 0.001f;
    properties.right_x = 0.5f;
    properties.right_y = 1.f/3.f + 0.001f;
    properties.back_x = 0.75f;
    properties.back_y = 1.f/3.f + 0.001f;
    properties.top_x = 0.25f;
    properties.top_y = 0.0f;
    properties.bottom_x = 0.25f;
    properties.bottom_y = 2.f/3.f + 0.001f;
    properties.width = 0.25f;
    properties.height = 1.f/3.f - 0.002f;
    properties.isBottom = true;
    return properties;
}
