#ifndef IMP_FREEFLYCAMERA_H
#define IMP_FREEFLYCAMERA_H

#define DIST_ACTION 300.f

#include "Camera.h"
#include <iostream>
#include "base/impBase.h"
#include "base/Timer.h"
#include "base/KeyBindingConfig.h"

IMPGEARS_BEGIN

/// \brief Defines a free fly camera.
/// key binding, names to use :
/// - forward
/// - back
/// - left
/// - right
/// - accelerate
/// - decelerate
class IMP_API FreeFlyCamera : public Camera
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

		void setKeyBindingConfig(const KeyBindingConfig& binding);

    protected:
    private:

		void setDefaultKeyBinding();

		/// Keys binding
		Uint32 m_forwardKey;
		Uint32 m_backKey;
		Uint32 m_leftKey;
		Uint32 m_rightKey;
		Uint32 m_accelerateKey;
		Uint32 m_decelerateKey;

        Timer m_cursorTimer;
        float xref, yref;
};

IMPGEARS_END

#endif // IMP_FREEFLYCAMERA_H
