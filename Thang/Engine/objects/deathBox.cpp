#include "deathBox.h"

using json = nlohmann::json;

DeathBox::DeathBox(sf::Vector2f size)
{
    setCollisionArea();
    m_size = size;
    object_type = std::string("DeathBox");
    sf::Shape::update();
}

void DeathBox::setSize(sf::Vector2f size)
{
    m_size = size;
    sf::Shape::update();
}
        
const sf::Vector2f& DeathBox::getSize() const
{
    return m_size;
}
        
std::size_t DeathBox::getPointCount() const
{
    return 4;
}

sf::Vector2f DeathBox::getPoint(std::size_t index) const
{
    switch (index) {
    case 0:
        return sf::Vector2f(0, 0);
    case 1:
        return sf::Vector2f(m_size.x, 0);
    case 2:
        return sf::Vector2f(m_size.x, m_size.y);
    case 3:
        return sf::Vector2f(0, m_size.y);
    default:
        exit(1);
    }
}

void DeathBox::logic()
{
    // Change to event on collision later
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object *> bodies = objectManager->overlappedBodies(this);
    for (auto & object : bodies) {
        if (object->object_type.compare("Character") == 0) {
            // Change to death event later
            object->setPosition(-100, -100);
            //  SEND TO SPAWN POINT
        }
    }
}

json DeathBox::toClientJSON()
{
    json output = clientJSONHelper();
    output["Size"] = {m_size.x, m_size.y};

    return output;
}