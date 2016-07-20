#ifndef IMP_CAMERA_H
#define IMP_CAMERA_H

#include "System/EvnContextInterface.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Core/impBase.h"
#include "System/Entity.h"

IMPGEARS_BEGIN

//#define CONIC_FOV 1
//#define SPHERIC_FOV 1

/// \brief Defines a scene camera.
class IMP_API Camera : public Entity
{
    public:

        struct FrustumConfig
        {
            float frustum_near;
            float frustum_far;
            float frustum_fovy;
            float fovx;
            float fov;
            float depth;
            float sphereCenterDist;
            float sphereRadius;
            float tanfov;
            float tanfovx;
            float tanfovy;
        };

        Camera(bool active = true);
        virtual ~Camera();

        virtual void initialize();
        virtual void update();
        virtual void onEvent(const Event& evn);

        virtual void activate() {setActiveCamera(this);}
        virtual void lookAt();

        const imp::Vector3& getPosition() const{return m_position;}
        void setPosition(const imp::Vector3& position){m_position = position;}

        const imp::Vector3& getForwardVector() const{return m_orientation;}
        const imp::Vector3& getLateralVector() const{return m_lateralVector;}
        void setTarget(const imp::Vector3& target);

        void move(const imp::Vector3& move);
        void rotate(float theta, float phi);

        void initFrustum(float width, float height, float fovy, float nearDist, float farDist);

//        bool testHitBox(const imp::Vector3& position, const lut1n::HitBox& hitbox) const;
        bool testFov(float x, float y, float z, float r);

        void setMovable(bool movable){m_movable = movable;}
        bool isMovable() const{return m_movable;}

        void setMotionSensivity(float sensivity){m_motionSensivity = sensivity;}

        float getLateralAngle() const{return m_theta;}
        float getVerticalAngle() const{return m_phi;}

        static Camera* getActiveCamera() {return m_activeCamera;}
        static const imp::Vector3& getUpVector() {return m_upVector;}

        const Vector3 getVectorFromCursor(float x, float y);

        const Matrix4& getViewMatrix() const{return m_viewMatrix;}

    protected:

        void debugDraw();

        void updateLateralVector();
        void updateFov();
        void anglesToVector();

        static void setActiveCamera(Camera* camera){m_activeCamera = camera;}

    private:

        static const imp::Vector3 m_upVector;

        static Camera* m_activeCamera;

        imp::Vector3 m_position;
        imp::Vector3 m_orientation;
        imp::Vector3 m_target;

        //imp::Vector3 m_forwardVector;
        imp::Vector3 m_lateralVector;
        imp::Vector3 m_headVector;

        bool m_movable;
        float m_motionSensivity;

        float m_theta, m_phi;

        FrustumConfig m_frustumConf;

        /// interet ?
        imp::Vector3 m_frustumSphereCenter;

        Matrix4 m_viewMatrix;
};

IMPGEARS_END

#endif // IMP_CAMERA_H
