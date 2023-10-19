#include "objectManager.h"
#include <cstdio>

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

ObjectManager* ObjectManager::inst = NULL;


ObjectManager::ObjectManager() {}

ObjectManager* ObjectManager::get() {
    if (inst == NULL) {
        inst = new ObjectManager();
    }
    return inst;
}

std::vector<Object*> ObjectManager::objects;

std::vector<PhysicsAffected*> ObjectManager::phys;

std::vector<Generic*> ObjectManager::generics;

void ObjectManager::addObject(Object *object)
{
    objects.push_back(object);
    if (instanceof<PhysicsAffected>(object)) {
        phys.push_back((PhysicsAffected *)object);
    } else if (instanceof<Generic>(object)) {
        generics.push_back((Generic *)object);
    }
}

void ObjectManager::removeObject(Object *object)
{
    objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
    if (instanceof<PhysicsAffected>(object)) {
        phys.erase(std::remove(phys.begin(), phys.end(), (PhysicsAffected *) object), phys.end());
    } else if (instanceof<Generic>(object)) {
        generics.erase(std::remove(generics.begin(), generics.end(), (Generic *) object), generics.end());
    }
}

std::vector<Object*> ObjectManager::getObjects() {
    return objects;
}

// wrapper function for the threads that we create below
void ObjectManager::physicsLogicWrapper()
{
    for(auto & physics : phys) {
        physics->logic();
    }
}

// wrapper function for the threads that we create below
void ObjectManager::genericLogicWrapper()
{
    for(auto & generic : generics) {
        generic->logic();
    }
}

void ObjectManager::updateObjects()
{
    std::vector<std::thread> threads;
    // threading the logic of the objects
    threads.push_back(std::thread(physicsLogicWrapper));
    threads.push_back(std::thread(genericLogicWrapper));
    for (auto & thread : threads) {
        thread.join();
    }
}

std::vector<Object*> ObjectManager::overlapped(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isOverlapped(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

//The five below functions just find all of the objects that are touching a certain object
//Utilizing the isTouching function in Object

std::vector<Object*> ObjectManager::touching(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isTouching(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBelow(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isTouchingBelow(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingRight(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isTouchingRight(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingLeft(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isTouchingLeft(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingAbove(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & other : objects)
    {
        if (other != object) {
            if (object->isTouchingAbove(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<PhysicsAffected*> ObjectManager::overlappedPhysics(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isOverlapped(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

//The five below functions just find all of the physicsAffect objects that are touching a certain object
//Utilizing the isOverlapped function in Object
std::vector<PhysicsAffected*> ObjectManager::touchingPhysics(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isTouching(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<PhysicsAffected*> ObjectManager::touchingPhysicsBelow(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isTouchingBelow(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<PhysicsAffected*> ObjectManager::touchingPhysicsRight(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isTouchingRight(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<PhysicsAffected*> ObjectManager::touchingPhysicsLeft(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isTouchingLeft(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<PhysicsAffected*> ObjectManager::touchingPhysicsAbove(Object *object)
{
    std::vector<PhysicsAffected*> touchers;
    for (auto & other : phys)
    {
        if (other != object) {
            if (object->isTouchingAbove(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}