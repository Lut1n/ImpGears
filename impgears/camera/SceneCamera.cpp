#include "SceneCamera.h"

#include "graphics/GLcommon.h"
#include <cmath>
#include <iostream>

IMPGEARS_BEGIN

//#define CAMERA_DEBUG

#define STRAT_CAM_HEIGHT 70.f

const imp::Vector3 Camera::m_upVector(0.f, 0.f, 1.f);

Camera* Camera::m_activeCamera = IMP_NULL;

Camera::Camera(bool active)
{
    if(active)
        activate();
}

Camera::~Camera()
{
}

void Camera::initialize(){
}

void Camera::onEvent(const imp::Event& evn){
}

void Camera::update(){
}

void Camera::lookAt()
{
    imp::Vector3 pos = m_position;
    imp::Vector3 target = m_position + m_orientation;

    #ifdef CAMERA_DEBUG
    fprintf(stdout, "<debug> CAM_POS {%f; %f; %f}\n", m_position.getX(), m_position.getY(), m_position.getZ());
    fprintf(stdout, "<debug> CAM_TARGET {%f; %f; %f}\n", target.getX(), target.getY(), target.getZ());
    pos = m_position - m_orientation*100.f;
    target = m_position;
     #endif

    gluLookAt(pos.getX(), pos.getY(), pos.getZ(),
              target.getX(),target.getY(), target.getZ(),
              m_upVector.getX(), m_upVector.getY(), m_upVector.getZ());
}

void Camera::setTarget(const imp::Vector3& target)
{
    m_target = target;

    m_orientation = m_target - m_position;
    m_orientation.normalize();
}

void Camera::move(const imp::Vector3& move)
{
    m_position = m_position + move;
}

void Camera::rotate(float theta, float phi)
{
    m_theta += theta;
    m_phi += phi;

    if(m_phi > 3.14f/2.f)m_phi = 3.14f/2.f;
    else if(m_phi < -3.14f/2.f)m_phi = -3.14f/2.f;

    m_orientation.setRadial(m_theta, m_phi);
}

void Camera::initFrustum(float width, float height, float fovy, float nearDist, float farDist)
{
    float w2 = width/2.f;
    float h2 = height/2.f;
    float screenCorner = sqrtf(w2*w2 + h2*h2);

    m_frustumConf.frustum_near = nearDist;
    m_frustumConf.frustum_far = farDist;
    m_frustumConf.frustum_fovy = fovy/2.f;
    m_frustumConf.fovx = (fovy * width/height) / 2.f;
    m_frustumConf.depth = h2 / tanf( fovy/2.f );
    m_frustumConf.fov = atan2( screenCorner, m_frustumConf.depth);

    float farH = tanf(m_frustumConf.fov)*farDist;
    float cent2far = (farDist - nearDist)/2.f;
    m_frustumConf.sphereCenterDist =  cent2far + nearDist;
    m_frustumConf.sphereRadius = sqrtf( cent2far*cent2far + farH*farH + farH*farH );
    m_frustumConf.tanfov = tanf(m_frustumConf.fov);

    m_frustumConf.tanfovx = tanf(m_frustumConf.fovx);
    m_frustumConf.tanfovy = tanf(m_frustumConf.frustum_fovy);
}

bool Camera::testFov(float x, float y, float z, float r)
{
    imp::Vector3 v(x,y,z);
    if(r >= 1.f)v = v + imp::Vector3(r,r,r);

    imp::Vector3 cam2pos =v - m_position;
    imp::Vector3 cam2posN(cam2pos); cam2posN.normalize();
    float camDist = cam2pos.getNorm();
    float depth = camDist * cam2posN.dotProduct(m_orientation);
    float effectiveR = sqrtf( r*r + r*r + r*r );
    float sumR = 0.f;

    ///distance culling///
    if(depth > m_frustumConf.frustum_far + effectiveR)return false;

    ///fps culling///
    if(//orientation.dot(cam2posN) < 0.f
       depth < 0.f && m_frustumConf.frustum_near - depth > effectiveR)return false; //because if cos negative (depth is negative)

    ///frustum culling (spheric)///
    #ifdef SPHERIC_FOV
    imp::Vector3 relSphere = cam2pos - m_frustumSphereCenter;
    float sphereSquareDist = relSphere.getSqNorm();
    sumR = (m_frustumConf.sphereRadius+effectiveR);
    if( sphereSquareDist >  sumR*sumR )return false;
    #endif

    #ifdef CONIC_FOV
    ///frustum culling (conic)///
    float r_local = (m_frustumConf.tanfov*depth);
    imp::Vector3 projectionTotal(m_orientation * depth);
    imp::Vector3 distVect( projectionTotal - cam2pos );
    float dist = distVect.getSqNorm();
    sumR = (r_local+effectiveR);
    if( dist > sumR*sumR )return false;

    #else
     ///frustum culling (pyramide fovX)///
     /*{
        float r_local = (m_frustumConf.tanfovx*depth);
        float dist = camDist * cam2posN.dotProduct(m_lateralVector);
        sumR = (r_local+effectiveR);
        if( dist*dist > sumR*sumR )return false;
     }*/
     ///frustum culling (pyramide fovY)///
     /*{
        float r_local = (m_frustumConf.tanfovy*depth);
        float dist = camDist * cam2posN.dotProduct(m_headVector);
        sumR = (r_local+effectiveR);
        if( dist*dist > sumR*sumR )return false;
     }*/
     ///frustum culling (pyramide fovX)///
     {
        float r_local = (m_frustumConf.tanfovx*depth);
        float dist = camDist * cam2posN.dotProduct(m_lateralVector);
        sumR = (r_local+effectiveR);
        if( dist*dist > sumR*sumR )return false;
     }
     ///frustum culling (pyramide fovY)///
     {
        float r_local = (m_frustumConf.tanfovy*depth);
        float dist = camDist * cam2posN.dotProduct(m_headVector);
        sumR = (r_local+effectiveR);
        if( dist*dist > sumR*sumR )return false;
     }
    #endif

    return true;
}

void Camera::debugDraw()
{
    #ifdef CAMERA_DEBUG
    glPushMatrix();
    glTranslatef(m_position.getX(), m_position.getY(), m_position.getZ());

    glBegin(GL_QUADS);
    glColor3f(0.f,1.f,1.f);
    glVertex3f(-0.2f,0.f,-0.2f);
    glColor3f(0.f,1.f,1.f);
    glVertex3f(-0.2f,0.f,0.2f);
    glColor3f(0.f,1.f,1.f);
    glVertex3f(0.2f,0.f,0.2f);
    glColor3f(0.f,1.f,1.f);
    glVertex3f(0.2f,0.f,-0.2f);
    glEnd();

    glPopMatrix();
    #endif
}

void Camera::updateLateralVector()
{
    m_lateralVector = m_orientation.crossProduct(m_upVector);
    m_lateralVector.normalize();
}

void Camera::updateFov()
{
    m_orientation.normalize();

    m_frustumSphereCenter =  ( m_orientation * m_frustumConf.sphereCenterDist ); ///pass

    updateLateralVector();
    m_headVector = m_lateralVector.crossProduct(m_orientation);
    m_headVector.normalize();
}

const Vector3 Camera::getVectorFromCursor(float x, float y)
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    GLdouble vx, vy, vz;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLint result = gluUnProject(x, viewport[3]-y, 0, modelview, projection, viewport, &vx, &vy, &vz);


    ///espace camera
    imp::Vector3 cam = Camera::getActiveCamera()->getPosition();
    vx -= cam.getX();
    vy -= cam.getY();
    vz -= cam.getZ();

    ///projection du point a la distance du sol
    vx *= -STRAT_CAM_HEIGHT/vz;
    vy *= -STRAT_CAM_HEIGHT/vz;
    vz = -STRAT_CAM_HEIGHT;

    ///retour a l'espace monde
    vx += cam.getX();
    vy += cam.getY();
    vz += cam.getZ();

    if(result == GL_FALSE){
        //error
        return Vector3(0.f, 0.f, 0.f);
    }

    return Vector3(vx, vy, vz);
}

IMPGEARS_END
