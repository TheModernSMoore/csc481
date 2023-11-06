#include "platform.h"

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

Platform::Platform(sf::Vector2f size) : Generic()
{
    this->m_size = size;
    sf::Shape::update();
}

void Platform::setSize(sf::Vector2f size)
{
    this->m_size = size;
    sf::Shape::update();
}
        
const sf::Vector2f& Platform::getSize() const
{
    return m_size;
}
        
std::size_t Platform::getPointCount() const
{
    return 4;
}

sf::Vector2f Platform::getPoint(std::size_t index) const
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

void Platform::setSpeed(float speedx, float speedy)
{
    this->speedx = speedx;
    this->speedy = speedy;
}

void Platform::setRange(float rangex, float rangey)
{
    this->rangex = rangex;
    this->rangey = rangey;
}

void Platform::setPauseTime(float pause)
{
    this->pause = pause;
    this->relTime = pause;
}

void Platform::logic()
{
    if (speedx != 0 || speedy != 0)
    {
        if (relTime < pause) {
            relTime++;
        } else {
            if ((relxPos >= rangex && speedx > 0) || (relxPos < 0 && speedx < 0))
            {
                speedx *= -1;
                relTime = 0;
            }
            if ((relyPos >= rangey && speedy > 0) || (relyPos < 0 && speedy < 0))
            {
                speedy *= -1;
                relTime = 0;
            }
            TimeManager *timeManager = TimeManager::get();
            Timeline *localTime = timeManager->getTimelines().at(1);
            float delta_time = localTime->deltaTime();
            move(speedx * delta_time, speedy * delta_time);
            //I think this kind of check is ok to not be immediately reliant, could be cringe
            ObjectManager *objectManager = ObjectManager::get();
            std::vector<PhysicsAffected*> objectsAbove = objectManager->touchingPhysicsAbove(this);
            for (auto & object : objectsAbove) {
                object->move(speedx * delta_time, speedy * delta_time);
            }

            relxPos += speedx * delta_time;
            relyPos += speedy * delta_time;
        }
    }
    sf::Shape::update();
}