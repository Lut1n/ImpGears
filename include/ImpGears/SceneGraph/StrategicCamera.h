#ifndef IMP_STRATEGICCAMERA_H
#define IMP_STRATEGICCAMERA_H

#include <SceneGraph/OpenGL.h>
#include <SceneGraph/Camera.h>
#include "Core/KeyBindingConfig.h"

IMPGEARS_BEGIN

/// \brief Defines a strategic camera.
/// key binding, names to use :
/// - forward
/// - back
/// - left
/// - right
/// - zoomIn
/// - zoomOut
class IMP_API StrategicCamera : public Camera
{
    public:
        StrategicCamera();
        virtual ~StrategicCamera();

        virtual void initialize();
        virtual void onEvent(const imp::Event& evn);
        virtual void update();

		void setKeyBindingConfig(const KeyBindingConfig& binding);

    protected:

    private:

		void setDefaultKeyBinding();

		/// Keys binding
		Uint32 m_forwardKey;
		Uint32 m_backKey;
		Uint32 m_leftKey;
		Uint32 m_rightKey;
		Uint32 m_zoomInKey;
		Uint32 m_zoomOutKey;
};

IMPGEARS_END

#endif // IMP_STRATEGICCAMERA_H
