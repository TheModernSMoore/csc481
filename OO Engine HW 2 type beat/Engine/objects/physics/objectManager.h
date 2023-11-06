#pragma once
#include <vector>
#include <typeinfo>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "physicsAffected.h"
#include "generic.h"
#include "../../timeline/timeManager.h"

/// declaration of Generic so the manager can use it 
class Generic;
/// declaration of PhysicsAffected so the manager can use it 
class PhysicsAffected;
/// the class that manages the interactions of each object in the game (or at least the current scope) 
class ObjectManager
{   
    private:
        /// the static list of game objects
        static std::vector<Object*> objects;

        static std::vector<PhysicsAffected*> phys;

        static std::vector<Generic*> generics;

        static ObjectManager* inst;

        /// empty constructor as this class is only used for its static list of objects
        /// and the interactions defined below
        ObjectManager();

        static void physicsLogicWrapper();

        static void genericLogicWrapper();

    public: 
        static ObjectManager* get();
        // adds a game object to the objects vector
        void addObject(Object *object);
        // removes a game object from the objects vector
        void removeObject(Object *object);
        // updates all game objects
        void updateObjects();
        // returns the vector of Object pointers
        std::vector<Object*> getObjects();
        //The five below functions just find all of the objects that are touching a certain object
        //Utilizing the isTouching function in Object
        std::vector<Object*> overlapped(Object *object);

        std::vector<Object*> touching(Object *object);

        std::vector<Object*> touchingBelow(Object *object);

        std::vector<Object*> touchingRight(Object *object);

        std::vector<Object*> touchingLeft(Object *object);

        std::vector<Object*> touchingAbove(Object *object);

        //The five below functions just find all of the physicsAffect objects that are touching a certain object
        //Utilizing the isTouching function in Object
        std::vector<PhysicsAffected*> overlappedPhysics(Object *object);

        std::vector<PhysicsAffected*> touchingPhysics(Object *object);

        std::vector<PhysicsAffected*> touchingPhysicsBelow(Object *object);

        std::vector<PhysicsAffected*> touchingPhysicsRight(Object *object);

        std::vector<PhysicsAffected*> touchingPhysicsLeft(Object *object);

        std::vector<PhysicsAffected*> touchingPhysicsAbove(Object *object);
};