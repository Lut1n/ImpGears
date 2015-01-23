#ifndef STRATEGICCAMERA_H
#define STRATEGICCAMERA_H

#include "graphics/GLcommon.h"
#include "camera/SceneCamera.h"

IMPGEARS_BEGIN

/// \brief Defines a strategic camera.
class StrategicCamera : public Camera
{
    public:
        StrategicCamera();
        virtual ~StrategicCamera();

        virtual void initialize();
        virtual void onEvent(const imp::Event& evn);
        virtual void update();
    protected:

    private:
};

IMPGEARS_END

#endif // STRATEGICCAMERA_H
