//
//  MathUtil.h
//  3D Math
//
//  Created by 刘宇望 on 16/10/1.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#ifndef MathUtil_h
#define MathUtil_h

#include <cmath>

enum TransType {
    object2world,
    world2object,
    object2inertial,
    inertial2object
};

const float kPi = 3.14159265f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi * 0.5f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;

inline float angle2radian (float a)
{
    return a * kPi / 180.0f;
}

inline float radian2angle (float r)
{
    return r * 180.0f / kPi;
}

inline float to_zero (float f)
{
    if (std::abs (f) < 0.00001f)
    {
        return 0.0f;
    }
    
    return f;
}

extern float wrapPi (float theta);

inline void sinCos (float *returnSin, float *returnCos, float theta)
{
    *returnSin = sin (theta);
    *returnCos = cos (theta);
}

extern float safeAcos (float x);


#endif /* MathUtil_h */
