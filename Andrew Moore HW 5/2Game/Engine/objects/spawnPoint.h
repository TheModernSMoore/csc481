#pragma once
#include "manager/objectManager.h"
#include "abtype/object.h"

class SpawnPoint : public Object
{
    public:
        SpawnPoint();

        virtual std::size_t getPointCount() const;
        
        virtual sf::Vector2f getPoint(std::size_t index) const;

        virtual void logic();

        void spawnHandle(Object* object);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
};