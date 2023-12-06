#include "character.h"
#include <iostream>


using json = nlohmann::json;

Character::Character(size_t radius ,float speed, float accel, float jump_speed) : Object()
{
    Object::setBody(true); // physics affected body
    Object::setVisible(); // allows to be drawn
    this->radius = radius;
    this->speed = speed;
    this->accel = accel;
    this->jump_speed = jump_speed;
    object_type = std::string("Character");
    sf::Shape::update();
}

void Character::setSize(size_t radius)
{
    this->radius = radius;
    sf::Shape::update();
}
        
const size_t Character::getSize() const
{
    return radius;
}
        
std::size_t Character::getPointCount() const
{
    return points;
}

void Character::setPointCount(int points)
{
    this->points = points;
    sf::Shape::update();
}

sf::Vector2f Character::getPoint(std::size_t index) const
{
    float angle = 2 * PI * index / points - PI/2;
    float x = radius * cos(angle);
    float y = radius * sin(angle);

    return sf::Vector2f(x, y);
}

// This handles gravity
void Character::logic()
{
    current_guid = guid;
    ScriptManager *sm = ScriptManager::get();
    sm->runOne("character_logic", true);
}

// This handles all input and corresponding movement
void Character::input(std::string direction)
{
    TimeManager *timeManager = TimeManager::get();
    Timeline *localTime = timeManager->getTimelines().at(1);
    float delta_time = localTime->deltaTime();
    if (direction.find("l") != std::string::npos) {
        if (body->velocity.x > 0) {
            body->velocity.x = 0;
        }
        // move(body->velocity.x <= speed * -1 ? body->velocity.x * delta_time : (body->velocity.x -= accel * delta_time) * delta_time, 0);
        body->velocity.x = body->velocity.x <= speed * -1 ? body->velocity.x : (body->velocity.x - accel * delta_time);
    } else if (direction.find("r") != std::string::npos) {
        if (body->velocity.x < 0) {
            body->velocity.x = 0;
        }
        // move(body->velocity.x >= speed ? body->velocity.x * delta_time : (body->velocity.x += accel * delta_time) * delta_time, 0);
        body->velocity.x = body->velocity.x >= speed ? body->velocity.x : (body->velocity.x + accel * delta_time);
    } else {
        body->velocity.x = 0;
    }
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> objectsBelow = objectManager->touchingBelow(this);
    // SHOULD PROBABLY JUST HAVE A ON GROUND FUNCTION
    if (objectsBelow.size() > 0 && direction.find("u") != std::string::npos) {
        body->velocity.y = jump_speed;
    }
}

json Character::toClientJSON()
{
    // Gets generic Object JSON
    json output = clientJSONHelper();
    output["Radius"] = radius;
    output["Speed"] = speed;
    output["Accel"] = accel;
    output["JumpSpeed"] = jump_speed;
    return output;
    
}
