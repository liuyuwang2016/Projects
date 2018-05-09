//
//  AABB.h
//  3D Math
//
//  Created by 刘宇望 on 11/15/16.
//  Copyright © 2016 刘宇望. All rights reserved.
//

#ifndef AABB_h
#define AABB_h

#include "Vector3M.hpp"

// Axis-aligned minimum bounding box
class AABB
{
public:
    Vector3M max;
    Vector3M min;
    
    Vector3M size () const { return max - min; }
    float xSize () { return max.x - min.x; }
    float ySize () { return max.y - min.y; }
    float zSize () { return max.z - min.z; }
    
    Vector3M center () { return (min + max) * 0.5f; }
};

#endif /* AABB_h */
