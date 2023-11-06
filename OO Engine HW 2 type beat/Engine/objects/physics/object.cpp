#include "object.h"

#include <iostream>

Object::Object() {
    objects_made++;
    identifier = objects_made;
}

Object::~Object() {}

void Object::logic() {}

std::mutex Object::movetex;

int Object::objects_made = 0;

bool Object::isOverlapped(Object *other)
{
    sf::Rect<float> shape1 = Shape::getGlobalBounds();
    sf::Rect<float> shape2 = other->Shape::getGlobalBounds();
    return shape1.intersects(shape2);
}

sf::Vector2f Object::distanceBetween(Object *other)
{
    sf::Rect<float> otherRect = other->getGlobalBounds();
    sf::Rect<float> thisRect = this->getGlobalBounds();
    float otherx = otherRect.left;
    float thisx = thisRect.left;
    float othery = otherRect.top;
    float thisy = thisRect.top;
    
    float x;
    if ((otherx > thisx && otherx < thisx + thisRect.width) || (otherx + otherRect.width > thisx && otherx + otherRect.width < thisx + thisRect.width) ||
        (thisx > otherx && thisx < otherx + otherRect.width) || (thisx + thisRect.width > otherx && thisx + thisRect.width < otherx + otherRect.width)) {
        x = std::numeric_limits<float>::max();
    } else {
        if (otherx >= thisx) {
            thisx += thisRect.width;
        } else {
            otherx += otherRect.width;
        }
        x = otherx - thisx;
    }

    float y;
    if ((othery > thisy && othery < thisy + thisRect.height) || (othery + otherRect.height > thisy && othery + otherRect.height < thisy + thisRect.height) ||
        (thisy > othery && thisy < othery + otherRect.height) || (thisy + thisRect.height > othery && thisy + thisRect.height < othery + otherRect.height)) {
        y = std::numeric_limits<float>::max();
    } else {
        if (othery >= thisy) {
            thisy += thisRect.height;
        } else {
            othery += otherRect.height;
        }
        y = othery - thisy;
    }
    return sf::Vector2f(x, y);
}

bool Object::isTouching(Object *other)
{
    sf::Vector2f distance = Object::distanceBetween(other);
    return distance.x == 0 || distance.y == 0;
}

//Checks if other is below this and touching.
bool Object::isTouchingBelow(Object *other)
{
    if (!isTouching(other))
        return false;
    sf::Rect<float> thisShape = Shape::getGlobalBounds();
    sf::Rect<float> otherShape = other->Shape::getGlobalBounds();
    return otherShape.top >= thisShape.top + thisShape.height;
}
//Checks if other is to the left of this and touching.
bool Object::isTouchingLeft(Object *other)
{
    if (!isTouching(other)) {
        return false;
    }
    sf::Rect<float> thisShape = Shape::getGlobalBounds();
    sf::Rect<float> otherShape = other->Shape::getGlobalBounds();
    return otherShape.left + otherShape.width >= thisShape.left;
}
//Checks if other is to the right of this and touching.
bool Object::isTouchingRight(Object *other)
{
    if (!isTouching(other)) {
        return false;
    }
    sf::Rect<float> thisShape = Shape::getGlobalBounds();
    sf::Rect<float> otherShape = other->Shape::getGlobalBounds();
    return otherShape.left <= thisShape.left + thisShape.width;
}
//Checks if other is above this and touching.
bool Object::isTouchingAbove(Object *other)
{
    if (!isTouching(other)) {
        return false;
    }
    sf::Rect<float> thisShape = Shape::getGlobalBounds();
    sf::Rect<float> otherShape = other->Shape::getGlobalBounds();
    return otherShape.top + otherShape.height <= thisShape.top;
}

std::string Object::toClientString()
{
    return std::string(std::to_string(identifier) + ": x: " + std::to_string(getPosition().x) + ", y: " + std::to_string(getPosition().y));
}

bool Object::parseString(std::string to_parse)
{
    int ident;
    float x_pos;
    float y_pos;
    if(sscanf(to_parse.c_str(), "%d: x: %f, y: %f", &ident, &x_pos, &y_pos) != 3 /**|| ident != identifier*/) {
        return false;
    }
    setPosition(x_pos, y_pos);
    return true;
}