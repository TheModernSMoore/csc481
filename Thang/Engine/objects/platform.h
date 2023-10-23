#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "manager/objectManager.h"
#include "abtype/object.h"

// The platform class, acts as the ground, floating platforms, walls, so on for character to interact with
class Platform : public Object
{
    private:
        // speed of platform in x direction
        float speedx = 0;
        // speed of platform in y direction
        float speedy = 0;
        // range of x motion
        float rangex = std::numeric_limits<float>::max();
        // range of y motion
        float rangey = std::numeric_limits<float>::max();
        // time that the platform will pause before going the other direction
        float pause = 0;
        // the relative time before the pause
        float relTime = 0;
        // the relative x position in relation to the rangex
        float relxPos = 0;
        // the relative y position in relation to the rangey
        float relyPos = 0;
        // the size of the platform
        sf::Vector2f m_size;

    public:
        // Constructor of platform (merely creates the rectangle shape based on size)
        Platform(sf::Vector2f size);
        // sets the platform size to the new vector
        void setSize(sf::Vector2f size);
        
        const sf::Vector2f& getSize() const;
        
        virtual std::size_t getPointCount() const;
        
        virtual sf::Vector2f getPoint(std::size_t index) const;

        void setSpeed(float speedx, float speedy);

        void setRange(float rangex, float rangey);

        void setPauseTime(float pause);

        virtual void logic();
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();

};