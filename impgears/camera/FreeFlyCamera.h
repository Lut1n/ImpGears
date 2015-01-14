#ifndef FREEFLYCAMERA_H
#define FREEFLYCAMERA_H

#define DIST_ACTION 300.f

#include "SceneCamera.h"
#include <iostream>
#include "base/impBase.hpp"
#include "base/Timer.h"

IMPGEARS_BEGIN

class FreeFlyCamera : public Camera
{
    public:
        FreeFlyCamera(float xref, float yref, const imp::Vector3& position = imp::Vector3(0.f, 0.f, 0.f), const imp::Vector3& target = imp::Vector3(1.f, 0.f, 0.f));
        virtual ~FreeFlyCamera();

        virtual void initialize();
        virtual void update();
        virtual void onEvent(imp::Event evn);

    protected:
    private:

        Timer m_cursorTimer;

        float xref, yref;
};

IMPGEARS_END

#endif // FREEFLYCAMERA_H
