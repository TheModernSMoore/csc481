#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "physics/objectManager.h"

#define PI 3.14159265

// Character class that will be the controlled character and is affected by Physics
class Character : public PhysicsAffected
{    
    private:
        // the current speed that is kept track of
        float curr_speed = 0;
        // the given speed of the character
        float speed;
        // the (horizontal) acceleration of the character
        float accel;
        // the jump speed/height of the character
        float jump_speed;
        // the vertical_speed to keep track of
        float vertical_speed = 0;
        // the radius of the character (circle)
        float radius;
        // the number of points the character has (for constructing the circle)
        int points = 30;

    public:
        // constructor for the character given radius, speed, acceleration, jump speed, and downwards acceleration
        Character(size_t radius, float speed, float accel, float jump_speed, float down_accel);
        // sets the size of the character given the radius
        void setSize(size_t radius);
        // gets the size (raidus) of the character
        const size_t getSize() const;
        // gets the point count of the circle shape created
        virtual std::size_t getPointCount() const;
        // gets the points of the circle shape created
        virtual sf::Vector2f getPoint(std::size_t index) const;
        // sets the amount of points needed to draw the circle (higher means smoother)
        void setPointCount(int points);
        // handles the input coming from the client
        void input(std::string direction);
        // the logic function required by Object to handle player input and movement
        virtual void logic();

};