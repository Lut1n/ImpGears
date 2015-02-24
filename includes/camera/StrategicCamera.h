#ifndef IMP_STRATEGICCAMERA_H
#define IMP_STRATEGICCAMERA_H

#include "graphics/GLcommon.h"
#include "camera/Camera.h"

IMPGEARS_BEGIN

/// \brief Defines a strategic camera.
class IMP_API StrategicCamera : public Camera
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

#endif // IMP_STRATEGICCAMERA_H
