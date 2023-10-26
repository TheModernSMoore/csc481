#pragma once
#include <vector>
#include <typeinfo>
#include <thread>
#include <SFML/Graphics.hpp>
#include "../abtype/object.h"
#include "../character.h"
#include "../platform.h"
#include "../spawnPoint.h"
#include "../deathBox.h"
#include "../../timeline/timeManager.h"
#include "../common.h"

class Object;

/// the class that manages the interactions of each object in the game (or at least the current scope) 
class ObjectManager
{   
    private:
        /// the static list of game objects
        static std::map<int, Object*> objects;

        static std::map<int, Object*> bodies;

        static std::map<int, Object*> visibles;

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
        // will parse json and if an object with the given identifier doesn't exist, it will make it
        // then updates position of matching object and returns it
        bool parseObjJSON(nlohmann::json to_parse);
        // returns the vector of Object pointers
        std::map<int, Object*> getObjects();
        // returns the map of visible Object pointers sorted by the identifier assigned
        std::map<int, Object*> getVisibles();

        std::map<int, Object*> getBodies();
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
        std::vector<Object*> overlappedPhysics(Object *object);

        std::vector<Object*> touchingPhysics(Object *object);

        std::vector<Object*> touchingPhysicsBelow(Object *object);

        std::vector<Object*> touchingPhysicsRight(Object *object);

        std::vector<Object*> touchingPhysicsLeft(Object *object);

        std::vector<Object*> touchingPhysicsAbove(Object *object);

        //The five below functions just find all of the physicsAffect objects that are touching a certain object
        //Utilizing the isTouching function in Object
        std::vector<Object*> overlappedBodies(Object *object);

        std::vector<Object*> touchingBodies(Object *object);

        std::vector<Object*> touchingBodiesBelow(Object *object);

        std::vector<Object*> touchingBodiesRight(Object *object);

        std::vector<Object*> touchingBodiesLeft(Object *object);

        std::vector<Object*> touchingBodiesAbove(Object *object);
};