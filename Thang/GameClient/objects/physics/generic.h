#pragma once
#include "objectManager.h"
#include "object.h"


/// The generic class, extends object and only holds the move function of generic objects that have
/// base level game operations
class Generic : public Object
{
    friend class PhysicsAffected;

    protected:
        /// empty costructor
        Generic();
    public:
        /// move function that pushes all PhysicsAffected objects out of the way.
        virtual void move(float offsetX, float offsetY);
};