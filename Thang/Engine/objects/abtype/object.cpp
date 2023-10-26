#include "object.h"
#include "../manager/objectManager.h"

#include <iostream>

using json = nlohmann::json;

std::mutex Object::movetex;

int Object::objects_made = 0;

Object::Object()
{
    identifier = objects_made++;
}

std::string Object::getObjectType()
{
    return object_type;
}

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

void Object::setTexturePath(std::string texture_path) {
    tex_path = texture_path;
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

void Object::setBody(bool affected)
{
    body = new object_body;
    body->physicsAffected = affected;
    body->velocity.x = 0;
    body->velocity.y = 0;
}

void Object::setCollisionArea()
{
    // parts of struct to set
    collision_area = new object_collision_area;
}

void Object::setVisible()
{
    visible = new object_visible;
    visible->in_frame = true;
}



Object::~Object() {}

void Object::logic() {}

bool Object::isOverlapped(Object *other)
{
    sf::Rect<float> shape1 = this->getGlobalBounds();
    sf::Rect<float> shape2 = other->getGlobalBounds();
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

json Object::toClientJSON() { return nullptr;}

json Object::clientJSONHelper() {
    json output;
    output["Type"] = object_type;
    output["Identifier"] = identifier;
    // stuff in visible struct
    if (visible) {
        output["InFrame"] = visible->in_frame;
        sf::Color color = getFillColor();
        output["Color"] = {color.r, color.g, color.b};
        output["Texture"] = tex_path;
    }
    //
    // stuff in body struct should just be handled by object creation
    sf::Vector2f position = getPosition();
    output["Position"] = {position.x, position.y};
    return output;
}