#pragma once
#include <limits>
#include <mutex>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "../common.h"
#include <nlohmann/json.hpp>

#define DOWN_ACCEL 0.3
#define TERM_VEL -6

/// The generic object class that is the base of all game objects
class Object : public sf::Shape
{
    friend class objectManager;
    private:
        // Keeps track of how many drawable objects have been made (those sent to client) in order to make unique identifier
        static int objects_made;

    protected:
        // the mutex used for safe movement in all objects, hence protected and static
        static std::mutex movetex;

        void setBody(bool physicsAffected);

        void setVisible();

        void setCollisionArea();

        std::string object_type;

        // Could be in visible struct as well as color
        std::string tex_path;

    public:
        int identifier;

        object_body *body;

        object_visible *visible;

        object_collision_area *collision_area;


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
        // Makes json info to be used for sending to client
        nlohmann::json toClientJSON();
        //abstract function logic that all objects will have (all stuff that happens before draw)
        virtual void logic() = 0;
        // Overwrites move and does different behavior based off of components
        virtual void move(float offsetX, float offsetY);
        // 
        void setTexturePath(std::string texture_path);
};