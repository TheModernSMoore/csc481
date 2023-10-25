#include "spawnPoint.h"

using json = nlohmann::json;

SpawnPoint::SpawnPoint()
{
    object_type = std::string("SpawnPoint");
    sf::Shape::update();
}
        
std::size_t SpawnPoint::getPointCount() const
{
    return 1;
}

sf::Vector2f SpawnPoint::getPoint(std::size_t index) const
{
    switch (index) {
    case 0:
        return sf::Vector2f(0, 0);
    default:
        exit(1);
    }
}

void SpawnPoint::logic()
{

}

void SpawnPoint::spawnHandle(Object *object, size64_t timeout)
{
    // Handles set up to be sent to in logic (this will probably all be in logic once we have events)
    

}

json SpawnPoint::toClientJSON()
{
    json output = clientJSONHelper();
    output["Size"] = {m_size.x, m_size.y};

    return output;
}