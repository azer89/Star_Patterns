#ifndef AVECTOR_H
#define AVECTOR_H

#include <limits>
#include <cmath>

struct AVector
{
public:
    // x
    float x;

    // y
    float y;

    // custom
    //int index;

    // Default constructor
    AVector()
    {
        this->x = -1;
        this->y = -1;
        //this->index = -1;
    }

    // Constructor
    AVector(float x, float y)
    {
        this->x = x;
        this->y = y;
        //this->index = -1;
    }

    // Constructor
    /*AVector(AVector otherVector)
    {
        this->x = otherVector.x;
        this->y = otherVector.y;
        //this->index = -1;
    }*/

    // Scale a point
    AVector Resize(float val)
    {
        AVector newP;
        newP.x = this->x * val;
        newP.y = this->y * val;
        return newP;
    }

    // if a point is (-1, -1)
    bool Invalid()
    {
        if(((int)x) == -1 && ((int)y) == -1)
            { return true; }
        return false;
    }

    // Normalize
    AVector Norm()
    {
        float vlength = std::sqrt( x * x + y * y );
        return AVector(this->x / vlength, this->y / vlength);
    }

    // Euclidean distance
    float Distance(AVector other)
    {
        float xDist = x - other.x;
        float yDist = y - other.y;
        return sqrt(xDist * xDist + yDist * yDist);
    }

    // Euclidean distance
    float Distance(float otherX, float otherY)
    {
        float xDist = x - otherX;
        float yDist = y - otherY;
        return sqrt(xDist * xDist + yDist * yDist);
    }

    // squared euclidean distance
    float DistanceSquared(AVector other)
    {
        float xDist = x - other.x;
        float yDist = y - other.y;
        return (xDist * xDist + yDist * yDist);
    }

    // squared euclidean distance
    float DistanceSquared(float otherX, float otherY)
    {
        float xDist = x - otherX;
        float yDist = y - otherY;
        return (xDist * xDist + yDist * yDist);
    }

    // operator overloading
    AVector operator+ (const AVector& other) { return AVector(x + other.x, y + other.y); }

    // operator overloading
    AVector operator- (const AVector& other) { return AVector(x - other.x, y - other.y); }
    bool operator== (const AVector& other)
    { return (abs(this->x - other.x) < std::numeric_limits<float>::epsilon() && abs(this->y - other.y) < std::numeric_limits<float>::epsilon()); }

    // operator overloading
    bool operator!= (const AVector& other)
    { return (abs(this->x - other.x) >= std::numeric_limits<float>::epsilon() || abs(this->y - other.y) >= std::numeric_limits<float>::epsilon()); }

    // operator overloading
    AVector operator+= (const AVector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    // operator overloading
    AVector operator-= (const AVector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // operator overloading
    AVector operator/ (const float& val) { return AVector(x / val, y / val); }

    // operator overloading
    AVector operator* (const float& val) { return AVector(x * val, y * val); }

    // operator overloading
    AVector operator*= (const float& val)
    {
        x *= val;
        y *= val;
        return *this;
    }

    // operator overloading
    AVector operator/= (const float& val)
    {
        x /= val;
        y /= val;
        return *this;
    }

    // length of a vector
    float Length() { return sqrt(x * x + y * y); }

    // squared length of a vector
    float LengthSquared() { return x * x + y * y; }

    // dot product
    float Dot(AVector otherVector) { return x * otherVector.x + y * otherVector.y; }

    // cross product
    AVector Cross(AVector otherVector)
    {
        //U x V = Ux*Vy-Uy*Vx
        return AVector(x * otherVector.y, y * otherVector.x);
    }

    // linear dependency test
    bool IsLinearDependent(AVector otherVector)
    {
        float det = (this->x * otherVector.y) - (this->y * otherVector.x);
        if(det > -std::numeric_limits<float>::epsilon() && det < std::numeric_limits<float>::epsilon()) { return true; }
        return false;
    }

    // angle direction
    AVector DirectionTo(AVector otherVector)
    {
        return AVector(otherVector.x - this->x, otherVector.y - this->y);
    }
};

#endif // AVECTOR_H
