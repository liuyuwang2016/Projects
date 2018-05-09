//
//  Matrix4x3.hpp
//  3D Math
//
//  Created by 刘宇望 on 16/9/30.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#ifndef Matrix4x3_hpp
#define Matrix4x3_hpp

#include <iostream>
class Vector3M;

class Matrix3x3
{
public:
    float m11,  m12,   m13;
    float m21,  m22,   m23;
    float m31,  m32,   m33;
    
    void print ();
};

class Matrix4x3
{
public:
    float m11,  m12,   m13;
    float m21,  m22,   m23;
    float m31,  m32,   m33;
    float tx,   ty,    tz;
    
    Matrix4x3 ();
    
    // axis 1->x 2->y 3->z
    void setRotate (int axis, float theta);
    
    void setScale (const float kx, const float ky, const float kz);
    void setScale (const Vector3M &v);
    void setProject (const Vector3M &v);
    void setReflect (int axis, float k);
    void setReflect (const Vector3M &n);
    void setShear (int axis, float s, float t); // s and t decide the range of shear
    
    void zeroTranslation ();
    void setTranslation (const Vector3M &d);
    void setupTranslation (const Vector3M &d);
    
    void print ();
};

Matrix4x3 operator * (const Matrix4x3 &a, const Matrix4x3 &b);
Vector3M operator * (const Vector3M &v, const Matrix4x3 &m);

Matrix4x3 &operator *= (Matrix4x3 &a, const Matrix4x3 &b);
Vector3M &operator *= (Vector3M &v, const Matrix4x3 &m);

float determinant (const Matrix3x3 &m);
Matrix3x3 inverse (const Matrix3x3 &m);

Vector3M getTranslation (const Matrix4x3 &m);

#endif /* Matrix4x3_hpp */
