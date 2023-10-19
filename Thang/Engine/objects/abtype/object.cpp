#include "object.h"

#include <iostream>

Object::Object()
{
    object_body *body = NULL;
    object_visible *visible = NULL;
    object_collision_area *collision_area = NULL;
    // How to not initialze
}

std::mutex Object::movetex;

int Object::visible_objects_made = 0;

void movePhysAffected(float offsetX, float offsetY, Object *to_move, std::mutex *movetex)
{
    std::unique_lock<std::mutex> lock(*movetex); // mutex lock to ensure that all checks for collision are still valid before movement concludes
    to_move->sf::Shape::move(offsetX, offsetY);
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> touchers = objectManager->overlappedBodies(to_move);
    if (touchers.size() > 0)
    {
        to_move->sf::Shape::move(-offsetX, -offsetY);
        float leastx = offsetX;
        float leasty = offsetY;
        for (auto & object : touchers)
        {
            sf::Vector2f distance = to_move->distanceBetween(object);
            float x = distance.x;
            float y = distance.y;
            if (x != std::numeric_limits<float>::max()) {
                if (offsetX > 0) {
                    leastx = x >= 0 && x < leastx ? x : leastx;
                } else if (offsetX < 0) {
                    leastx = x <= 0 && x > leastx ? x : leastx;
                }
            }
            if (y != std::numeric_limits<float>::max()) {
                if (offsetY > 0) {
                    leasty = y >= 0 && y < leasty ? y : leasty;
                } else if (offsetY < 0) {
                    leasty = y <= 0 && y > leasty ? y : leasty;
                }
            }
        }
        to_move->sf::Shape::move(leastx, leasty);
    }
}

void movePhysUnaffected(float offsetX, float offsetY, Object *to_move, std::mutex *movetex)
{
    std::unique_lock<std::mutex> lock(*movetex); // mutex lock to ensure that all checks for collision are still valid before movement concludes
    to_move->sf::Shape::move(offsetX, offsetY);
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> touchers = objectManager->overlappedPhysics(to_move);
    if (touchers.size() > 0)
    {
        to_move->sf::Shape::move(-offsetX, -offsetY);
        for (auto & phy : touchers)
        {
            sf::Vector2f distance = to_move->distanceBetween(phy);
            float x = distance.x != std::numeric_limits<float>::max() ? distance.x : 0;
            float y = distance.y !=std::numeric_limits<float>::max() ? distance.y : 0;
            lock.unlock(); // this calls move on the affected object so we need to unlock the mutex.
            phy->move(offsetX - x, offsetY - y);
            lock.lock(); // lock it again
        }
        to_move->sf::Shape::move(offsetX, offsetY);
    }
}

void Object::setBody(bool affected)
{
    object_body body_toset;
    body_toset.physicsAffected = affected;
    body = &body_toset;
}

void Object::move(float offsetX, float offsetY) {
    if (!body) {
        Shape::move(offsetX, offsetY);
    } else {
        if (body->physicsAffected) {
            movePhysAffected(offsetX, offsetY, this, &movetex);
        } else {
            movePhysUnaffected(offsetX, offsetY, this, &movetex);
        }
    }
}




Object::~Object() {}

void Object::logic() {}

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
    if (visible) {
        return std::string(std::to_string(visible->identifier) + ": x: " + std::to_string(getPosition().x) + ", y: " + std::to_string(getPosition().y));
    }
    return std::string("you stupid.");
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