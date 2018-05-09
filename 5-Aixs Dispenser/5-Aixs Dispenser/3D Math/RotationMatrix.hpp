//
//  RotationMatrix.hpp
//  3D Math
//
//  Created by 刘宇望 on 16/10/17.
//  Copyright © 2016年 刘宇望. All rights reserved.
//

#ifndef RotationMatrix_hpp
#define RotationMatrix_hpp

class Vector3M;
class EulerAngle;
class Quaternion;

class RotationMatrix {
public:
    float m11, m12, m13;
    float m21, m22, m23;
    float m31, m32, m33;
    
    RotationMatrix (const EulerAngle &e);
    RotationMatrix (const Quaternion& q, TransType transType);
    
    void identity ();
    
    Vector3M inertial2Object (const Vector3M &v) const;
    Vector3M object2Inertial (const Vector3M &v) const;
};

#endif /* RotationMatrix_hpp */
