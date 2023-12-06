#pragma once
#include "manager/objectManager.h"
#include "abtype/object.h"

#define BULLET_SPEED 5

class DeathBox : public Object
{
    private:
        // the size of the death box
        sf::Vector2f m_size;

    public:
        bool player_created;

        DeathBox(sf::Vector2f size, bool player_created);

        // sets the death box size to the new vector
        void setSize(sf::Vector2f size);
        
        const sf::Vector2f& getSize() const;
        
        virtual std::size_t getPointCount() const;
        
        virtual sf::Vector2f getPoint(std::size_t index) const;

        virtual void logic();

        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
};