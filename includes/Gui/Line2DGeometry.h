#ifndef IMP_LINE2D_STATE_H
#define IMP_LINE2D_STATE_H

#include "Core/impBase.h"
#include "Graphics/VBOData.h"

IMPGEARS_BEGIN

class IMP_API Line2DGeometry : public VBOData
{
    public:

		//--------------------------------------------------------------
    	struct Point2d
    	{
    		float _x;
    		float _y;
    	};
		
        Line2DGeometry();
		
        virtual ~Line2DGeometry();
		
        void draw();
		
		void update();
		
        void setLine(Point2d& p1, Point2d& p2);

    protected:
    	Point2d	_p1;
    	Point2d	_p2;
		Uint64	_vertexBuffSize;
		bool	_needUpdate;
};


IMPGEARS_END

#endif // IMP_LINE2D_STATE_H
