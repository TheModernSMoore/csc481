#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "manager/objectManager.h"
#include "abtype/object.h"

#define ENEMY_SPEED 10

// The Enemy class, acts as the ground, floating Enemys, walls, so on for character to interact with
class Enemy : public Object
{
    private:
        // the size of the Enemy
        sf::Vector2f m_size;
        //  a vector of position vectors for the Enemy to travel to
        std::vector<sf::Vector2f> to_go;
        // int with index of the next position vector to go to
        int next_point = 0;
        // Determines the relative position of 
        float rel_pos = 0;
        // 0 = down, 1 = left, 2 = right
        int curr_dir = 0;
        // relative time
        int rel_time = 0;

    public:
        // Constructor of Enemy (merely creates the rectangle shape based on size)
        Enemy(sf::Vector2f size);
        // sets the Enemy size to the new vector
        void setSize(sf::Vector2f size);
        
        const sf::Vector2f& getSize() const;
        
        virtual std::size_t getPointCount() const;
        
        virtual sf::Vector2f getPoint(std::size_t index) const;

        virtual void logic();
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();

};