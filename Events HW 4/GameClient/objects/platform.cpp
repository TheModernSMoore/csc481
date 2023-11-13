#include "platform.h"

using json = nlohmann::json;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

Platform::Platform(sf::Vector2f size)
{
    Object::setBody(false); // not physics affected body
    Object::setVisible();
    m_size = size;
    object_type = std::string("Platform");
    sf::Shape::update();
}

void Platform::setSize(sf::Vector2f size)
{
    m_size = size;
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

void Platform::setSpeed(float speed) // PUT VECTORS HERE TOO
{
    this->speed = speed;
}

void Platform::setNextPoint(int next)
{
    this->next_point = next;
}

void Platform::setToGo(std::vector<sf::Vector2f> to_set)
{
    to_go = to_set;
}

sf::Vector2f normalize(sf::Vector2f to_norm)
{
    float magnitude = sqrt(to_norm.x * to_norm.x + to_norm.y * to_norm.y);
    return to_norm / magnitude;
}

void Platform::logic()
{
    if (speed != 0 && to_go.size() != 0)
    {
        // move towards next point
        sf::Vector2f direction = normalize(to_go.at(next_point) - getPosition());
        body->velocity = direction * speed;

        TimeManager *timeManager = TimeManager::get();
        Timeline *localTime = timeManager->getTimelines().at(1);
        float delta_time = localTime->deltaTime();

        move(body->velocity.x * delta_time, body->velocity.y * delta_time);
        if (normalize(to_go.at(next_point) - getPosition()) != direction) {
            next_point++;
            if (next_point >= to_go.size())
                next_point = 0;
        }
        //I think this kind of check is ok to not be immediately reliant, could be cringe
        ObjectManager *objectManager = ObjectManager::get();
        std::vector<Object*> objectsAbove = objectManager->touchingPhysicsAbove(this);
        for (auto & object : objectsAbove) {
            object->move(body->velocity.x * delta_time, body->velocity.y * delta_time);
        }
    }
    sf::Shape::update();
}

json Platform::toClientJSON()
{
    json output = clientJSONHelper();
    
    // Might have to get direction of velocity in this to send to client
    // server will handle direction changes, client will merely just move it in the chosen direction until told otherwise
    output["Size"] = {m_size.x, m_size.y};
    output["Speed"] = speed;
    output["NextPoint"] = next_point;
    output["PointAmount"] = to_go.size();
    int idx = 0;
    for (auto & point : to_go) {
        output[std::string("Point") + std::to_string(idx)] = {point.x, point.y};
        idx++;
    }

    return output;
    
}