#ifndef ASTARPATHFINDING_H
#define ASTARPATHFINDING_H

#include "Core/impBase.h"

#include <list>
#include <cstdlib>
#include <iostream>

class IMP_API Point_t
{
	public:
	    float x;
	    float y;
	    float costa;
	    float costb;
	    float totalcost;
	    Point_t* previous;
};

typedef std::list<Point_t*> PointList_t;

class IMP_API AstarPathfinding
{
    public:
        AstarPathfinding(bool *matrix, int w, int h, int distMax);
        virtual ~AstarPathfinding();

        void getPath(float srcX, float srcY, float destX, float destY, PointList_t &path);

        void clearList(PointList_t &list);

        static AstarPathfinding* getInstance(){return instance;}

    protected:
    private:
        int w, h;
        bool *matrix;
        int dist_max;
        PointList_t closedList, openedList;
        Point_t srcP, destP, *currentP;

        void perform();

        Point_t* existsIn(float x, float y, PointList_t* list);
        Point_t* getBest();

        static AstarPathfinding* instance;
};

#endif // ASTARPATHFINDING_H
