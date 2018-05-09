//
//  Quaternion.hpp
//  3D Math
//
//  Created by 刘宇望 on 16/10/19.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#ifndef Quaternion_hpp
#define Quaternion_hpp

class Vector3M;
class RotationMatrix;

class Quaternion
{
public:
    
    float w, x, y, z;
    
    // constructors
    Quaternion () {};
    Quaternion (float w, float x, float y, float z);
    Quaternion (const RotationMatrix& rm);
    Quaternion (const EulerAngle& orientation, TransType transType);
    
    // rotate around x axis
    void setRotationX (float theta);
    void setRotationY (float theta);
    void setRotationZ (float theta);
    void setRotationAxis (const Vector3M& axis, float theta);
    
    float getRotationAngle () const;
    Vector3M getRotationAxis () const;
    
    Quaternion operator * (const Quaternion& a) const;
    Quaternion& operator *= (const Quaternion& a);
    
    void identity () { w = 1.0f; x = y = z = 0.0f; }
    void normalize ();
};

extern const Quaternion kQuaternionIdentity;
extern float dotProduct (const Quaternion& a, const Quaternion& b);
extern Quaternion conjugate (const Quaternion& q);
extern Quaternion pow (const Quaternion& q, float exponent);
extern Quaternion slerp (const Quaternion& q0, const Quaternion& q1, float t);

#endif /* Quaternion_hpp */
