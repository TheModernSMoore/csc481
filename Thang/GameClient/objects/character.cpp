#include "character.h"
#include <iostream>


using json = nlohmann::json;

Character::Character(size_t radius ,float speed, float accel, float jump_speed)
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
    TimeManager *timeManager = TimeManager::get();
    Timeline *localTime = timeManager->getTimelines().at(1);
    float delta_time = localTime->deltaTime();
    //I think this kind of check is ok to not be immediately reliant, could be cringe
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> objectsBelow = objectManager->touchingBelow(this);
    std::vector<Object*> objectsAbove = objectManager->touchingAbove(this);
    if (objectsAbove.size() > 0) {
        vertical_speed = -DOWN_ACCEL * delta_time;
    } else if (objectsBelow.size() > 0) {
        vertical_speed = -DOWN_ACCEL * delta_time;
    } else if (vertical_speed > TERM_VEL) {
        vertical_speed -= DOWN_ACCEL * delta_time;
    }
    move(0, TERM_VEL <= vertical_speed ? -vertical_speed * delta_time : -TERM_VEL * delta_time);
}

// This handles all input and corresponding movement
void Character::input(std::string direction)
{
    TimeManager *timeManager = TimeManager::get();
    Timeline *localTime = timeManager->getTimelines().at(1);
    float delta_time = localTime->deltaTime();
    if (direction.find("l") != std::string::npos) {
        if (curr_speed > 0) {
            curr_speed = 0;
        }
        move(curr_speed <= speed * -1 ? curr_speed * delta_time : (curr_speed -= accel * delta_time) * delta_time, 0);
    } else if (direction.find("r") != std::string::npos) {
        if (curr_speed < 0) {
            curr_speed = 0;
        }
        move(curr_speed >= speed ? curr_speed * delta_time : (curr_speed += accel * delta_time) * delta_time, 0);
    }
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> objectsBelow = objectManager->touchingBelow(this);
    // SHOULD PROBABLY JUST HAVE A ON GROUND FUNCTION
    if (objectsBelow.size() > 0 && direction.find("u") != std::string::npos) {
        vertical_speed = jump_speed * delta_time;
        move(0, -vertical_speed);
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