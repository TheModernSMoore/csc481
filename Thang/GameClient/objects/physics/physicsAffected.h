#pragma once
#include <SFML/Graphics/Shape.hpp>
#include "object.h"
#include "objectManager.h"

// PhysicsAffected class, extends Object and implements movement based on game physics
//should probably delegate a gravity function to here
class PhysicsAffected : public Object
{

    protected:
        // the falling acceleration (as a pos float) of the object
        float down_accel;
        // the terminal velocity (it is negative ik that is confusing but my code works so :P)
        float term_vel = -6;
        // Constructor that specifies the down_accel of the object
        PhysicsAffected(float down_accel);
    
    public:
        // overriden move function to check for collision and resolve it by having this object stop
        virtual void move(float offsetX, float offsetY);
};