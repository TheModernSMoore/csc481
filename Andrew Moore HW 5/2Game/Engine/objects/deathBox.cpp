#include "deathBox.h"

using json = nlohmann::json;

DeathBox::DeathBox(sf::Vector2f size, bool player_created)
{
    setCollisionArea();
    setVisible();
    m_size = size;
    this->player_created = player_created;
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
    // PLACE THIS INSIDE OF CHECKCOLLISION OF COLLIDE AREA
    // Change to event on collision later
    
    TimeManager *timeManager = TimeManager::get();
    Timeline *localTime = timeManager->getTimelines().at(1);
    float delta_time = localTime->deltaTime();
    move(0, player_created ? BULLET_SPEED * delta_time * -1 : BULLET_SPEED * delta_time);
    collision_area->checkCollision(this);
    if (this->getPosition().y < -100 || this->getPosition().y > 700) {
        EventManager::get()->raise(new BulletDeath(this));
    }
}

json DeathBox::toClientJSON()
{
    json output = clientJSONHelper();
    output["Size"] = {m_size.x, m_size.y};

    return output;
}