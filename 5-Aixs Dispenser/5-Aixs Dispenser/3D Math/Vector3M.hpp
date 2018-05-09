//
//  Vector3.hpp
//  3D Math
//
//  Created by 刘宇望 on 16/9/20.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#ifndef Vector3_hpp
#define Vector3_hpp

#include <cmath>
#include <iostream>

#include "MathUtil.h"

class Vector3M
{
public:
    float x, y, z;
    
    Vector3M () {}
    Vector3M (const Vector3M &a) : x (a.x), y (a.y), z (a.z) {}
    Vector3M (float nx, float ny, float nz) : x (nx), y (ny), z (nz) {}
    
    void zero () { x = y = z = 0.0f; }
    
    Vector3M operator - () const;
    
    Vector3M operator * (float a) const;
    
    Vector3M operator *= (float a);
    
    Vector3M operator / (float a) const;
    
    Vector3M operator /= (float a);
    
    Vector3M operator + (Vector3M v) const;
    
    Vector3M operator += (Vector3M v);
    
    Vector3M operator - (Vector3M v) const;
    
    Vector3M operator -= (Vector3M v);
    
    float operator * (const Vector3M &a) const;
    
    void normalize ();
    
    void print ();
};

inline float vectorMag (const Vector3M &a)
{
    return sqrt (a.x * a.x + a.y * a.y + a.z * a.z);
}

// Must define a non-member function to do left *
inline Vector3M operator * (float k, const Vector3M v)
{
    return Vector3M (k * v.x, k * v.y, k * v.z);
}

inline float distance (const Vector3M v1, const Vector3M v2)
{
    return vectorMag(v1 - v2);
}

inline double calcAngle (const Vector3M v1, const Vector3M v2)
{
    double dp = v1 * v2;
    
    return acos(dp / (vectorMag(v1) * vectorMag(v2))) * 180.0 / 3.14;
}

inline Vector3M crossProduct (const Vector3M &v1, const Vector3M &v2)
{
    return Vector3M (
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    );
}

extern const Vector3M kZeroVector;

#endif /* Vector3M_hpp */
