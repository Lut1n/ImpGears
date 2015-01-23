#ifndef FREEFLYCAMERA_H
#define FREEFLYCAMERA_H

#define DIST_ACTION 300.f

#include "SceneCamera.h"
#include <iostream>
#include "base/impBase.hpp"
#include "base/Timer.h"

IMPGEARS_BEGIN

/// \brief Defines a free fly camera.
class FreeFlyCamera : public Camera
{
    public:

        /// \brief Constructor of the camera.
        /// \param xref - X axis center of the screen.
        /// \param yref - Y axis center of the screen.
        /// \param position - initial position of the camera.
        /// \param target - initial target of the camera.
        FreeFlyCamera(float xref, float yref, const imp::Vector3& position = imp::Vector3(0.f, 0.f, 0.f), const imp::Vector3& target = imp::Vector3(1.f, 0.f, 0.f));
        virtual ~FreeFlyCamera();

        virtual void initialize();

        virtual void update();

        virtual void onEvent(const imp::Event& evn);

    protected:
    private:

        Timer m_cursorTimer;

        float xref, yref;
};

IMPGEARS_END

#endif // FREEFLYCAMERA_H
