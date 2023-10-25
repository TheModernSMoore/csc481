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
        float speed = 0;
        // the size of the platform
        sf::Vector2f m_size;
        //  a vector of position vectors for the platform to travel to
        std::vector<sf::Vector2f> to_go;
        // int with index of the next position vector to go to
        int next_point = 0;

    public:
        // Constructor of platform (merely creates the rectangle shape based on size)
        Platform(sf::Vector2f size);
        // sets the platform size to the new vector
        void setSize(sf::Vector2f size);
        
        const sf::Vector2f& getSize() const;
        
        virtual std::size_t getPointCount() const;
        
        virtual sf::Vector2f getPoint(std::size_t index) const;
        // sets speed of platform, and tells it what position vectors to go to
        void setSpeed(float speed);
        
        void setNextPoint(int next);

        void setToGo(std::vector<sf::Vector2f> to_set);

        virtual void logic();
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();

};