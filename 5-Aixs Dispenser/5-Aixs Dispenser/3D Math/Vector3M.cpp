//
//  Vector3M.cpp
//  3D Math
//
//  Created by 刘宇望 on 16/9/20.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#include "3DMath.h"

const Vector3M kZeroVector (0.0f, 0.0f, 0.0f);

void Vector3M::print ()
{
    std::cout << "x:\t" << to_zero (x) << "\ty:\t" << to_zero (y) << "\tz:\t" << to_zero (z) << std::endl;
}

Vector3M Vector3M::operator - () const
{
    return Vector3M (-x, -y, -z);
}

Vector3M Vector3M::operator * (float a) const
{
    return Vector3M (a * x, a * y, a * z);
}

Vector3M Vector3M::operator *= (float a)
{
    x *= a; y *= a; z *= a;
    return *this;
}

Vector3M Vector3M::operator / (float a) const
{
    float n = 1.0f / a;
    
    return Vector3M (n * x, n * y, n * z);
}

Vector3M Vector3M::operator /= (float a)
{
    float n = 1.0f / a;
    x *= n; y *= n; z *= n;
    return *this;
}

void Vector3M::normalize ()
{
    float magSq = x*x + y*y + z*z;
    
    if (magSq > 0.0f)
    {
        float n = 1.0f / sqrt (magSq);
        x *= n;
        y *= n;
        z *= n;
    }
}

Vector3M Vector3M::operator + (Vector3M v) const
{
    return Vector3M (x + v.x, y + v.y, z + v.z);
}

Vector3M Vector3M::operator += (Vector3M v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    
    return *this;
}

Vector3M Vector3M::operator - (Vector3M v) const
{
    return Vector3M (x - v.x, y - v.y, z - v.z);
}

Vector3M Vector3M::operator -= (Vector3M v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

float Vector3M::operator * (const Vector3M &v) const
{
    return x * v.x + y * v.y + z * v.z;
}
