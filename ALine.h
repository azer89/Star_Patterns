#ifndef ALINE_H
#define ALINE_H

#include "AVector.h"

#include <limits>
#include <cmath>

struct ALine
{
public:
    float XA;	float YA;	// start
    float XB;	float YB;	// end

    // custom
    //int index1;
    //int index2;

    // Constructor #1
    ALine()
    {
        this->XA = -1;	this->YA = -1;
        this->XB = -1;	this->YB = -1;

        //this->index1 = -1;
        //this->index2 = -1;
    }

    // Constructor #2
    ALine(float XA, float YA, float XB, float YB)
    {
        this->XA = XA;	this->YA = YA;
        this->XB = XB;	this->YB = YB;

        //this->index1 = -1;
        //this->index2 = -1;
    }

    // Constructor #3
    /*ALine(ALine otherLine)
    {
        this->XA = otherLine.XA;	this->YA = otherLine.YA;
        this->XB = otherLine.XB;	this->YB = otherLine.YB;

        //this->index1 = -1;
        //this->index2 = -1;
    }*/

    ALine(AVector v1, AVector v2)
    {
        this->XA = v1.x;	this->YA = v1.y;
        this->XB = v2.x;	this->YB = v2.y;

        //this->index1 = -1;
        //this->index2 = -1;
    }


    ALine Resize(float val)
    {
        ALine newL;

        newL.XA = this->XA * val;
        newL.YA = this->YA * val;

        newL.XB = this->XB * val;
        newL.YB = this->YB * val;

        return newL;
    }

    // Uninitialized ?
    bool Invalid()
    {
        if(((int)XA) == -1 && ((int)YA) == -1 && ((int)XB) == -1 && ((int)YB) == -1) return true;
        return false;
    }

    // Start point
    AVector GetPointA() { return AVector(XA, YA); }

    // End point
    AVector GetPointB() { return AVector(XB, YB); }

    // Direction of the vector
    AVector Direction() { return AVector(XB - XA, YB - YA);}

    float Magnitude()
    {
        AVector dir = Direction();
        return sqrt(dir.x * dir.x + dir.y * dir.y);
    }

    bool LiesHere(AVector vec)
    {
        float det = (XB - XA) * (vec.y - YA) - (YB - YA) * (vec.x - XA);
        if(det > -1e-8 && det < 1e-8) return true;
        return false;
    }

    // returns:
    //	 1 : same direction
    //  -1 : opposite direction
    //   0 : other else
    int HasSameDirection(ALine otherLine)
    {
        float mag1 = Magnitude();
        float mag2 = otherLine.Magnitude();

        AVector dir1 = Direction();
        AVector dir2 = otherLine.Direction();

        float a_dot_b = dir1.Dot(dir2);
        float a_b_mag = mag1 *  mag2;

        float addValue = a_dot_b + a_b_mag;
        if(addValue > -std::numeric_limits<float>::epsilon() && addValue < std::numeric_limits<float>::epsilon() ) { return -1; }

        float subsValue = a_dot_b - a_b_mag;

        if(subsValue > -std::numeric_limits<float>::epsilon() && subsValue < std::numeric_limits<float>::epsilon() ) { return 1; }

        return 0;
    }
};

#endif // ALINE_H
