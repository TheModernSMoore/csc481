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
json SpawnPoint::toClientJSON()
{
    json output = clientJSONHelper();

    return output;
}