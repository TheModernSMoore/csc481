#pragma once
#include <limits>
#include <mutex>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Rect.hpp>

/// The generic object class that is the base of all game objects
class Object : public sf::Shape
{
    private:
        // Keeps track of how many objects have been made in order to make unique identifier
        static int objects_made;
        int identifier;

    protected:
        // the mutex used for safe movement in all objects, hence protected and static
        static std::mutex movetex;

    public:
        //Constructor and destructor below
        Object();
        virtual ~Object() = 0;
        //checks if objects are overlapped
        bool isOverlapped(Object *other);
        //returns smallest distance between this and other
        sf::Vector2f distanceBetween(Object *other);
        //checks if the distance between is 0
        bool isTouching(Object *other);
        //Checks if other is below this and touching.
        bool isTouchingBelow(Object *other);
        //Checks if other is to the left of this and touching.
        bool isTouchingLeft(Object *other);
        //Checks if other is to the left of this and touching.
        bool isTouchingRight(Object *other);
        //Checks if other is above this and touching.
        bool isTouchingAbove(Object *other);
        // Makes string info to be used for sending to client
        std::string toClientString();
        // Utilizes the above string to position the object (if able, returns false if not)
        bool parseString(std::string to_parse);
        //abstract function logic that all objects will have (all stuff that happens before draw)
        virtual void logic() = 0;
};