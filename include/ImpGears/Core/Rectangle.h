#ifndef IMP_RECTANGLE_H
#define IMP_RECTANGLE_H

#include <Core/Object.h>

IMPGEARS_BEGIN

/// \brief Defines a float rectangle.
class IMP_API Rectangle : public Object
{
	public:
	
	
	Meta_Class(Rectangle)
	
	Rectangle();
	Rectangle(const Rectangle& other);
	Rectangle(float x1, float y1, float x2, float y2);
	
	virtual ~Rectangle();
	
	void copy(const Rectangle& other);
	
	const Rectangle& operator=(const Rectangle& other);
	
	Rectangle operator+(const Rectangle& other) const;

	void expendBy(const Rectangle& other);
	
	void intersect(const Rectangle& other);
	
	bool contains(float x, float y) const;
	
	float getX1() const {return _x1;}
	float getY1() const {return _y1;}
	float getX2() const {return _x2;}
	float getY2() const {return _y2;}
	
	protected:
	
	float _x1;
	float _y1;
	float _x2;
	float _y2;
};

IMPGEARS_END

#endif // IMP_RECTANGLE_H
