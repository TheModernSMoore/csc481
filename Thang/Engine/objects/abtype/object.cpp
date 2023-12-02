#include "object.h"
#include "../manager/objectManager.h"

#include <iostream>

#include <filesystem>
#include <cstring>
#include "../../ScriptManager.h"
#include "../../v8helpers.h"

using json = nlohmann::json;

std::mutex Object::movetex;
std::mutex Object::createtex;
std::string Object::current_guid;

int Object::objects_made = 0;

Object::Object()
{
    std::unique_lock<std::mutex> lock(createtex);
    identifier = objects_made++;
    guid = std::string("Object") + std::to_string(identifier);
    current_guid = guid;
    ScriptManager::get()->runOne("hello_world", false);
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

void checkCollision(Object *toCheck) {
    ObjectManager *objectManager = ObjectManager::get();
    std::vector<Object*> tokill;
    for (auto & object : objectManager->overlappedBodies(toCheck)) {
        if (object->getObjectType().compare("Character") == 0) {
            // Change to death event later
            EventManager::get()->raise(new CharacterCollision((Character *)object, toCheck));
        }
    }
}

void Object::setCollisionArea()
{
    // parts of struct to set
    collision_area = new object_collision_area;
    collision_area->checkCollision = checkCollision;
}

void Object::setVisible()
{
    visible = new object_visible;
    visible->in_frame = true;
}



Object::~Object() {
    context->Reset();
}

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

// v8 stuff below

/**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work. 
 */
v8::Local<v8::Object> Object::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("x", getObjectX, setObjectX));
	v.push_back(v8helpers::ParamContainer("y", getObjectY, setObjectY));
	v.push_back(v8helpers::ParamContainer("guid", getObjectGUID, setObjectGUID));
    v.push_back(v8helpers::ParamContainer("curr_guid", getObjectCurrGUID, setObjectCurrGUID));
    if (body) {
        v.push_back(v8helpers::ParamContainer("velX", getObjectVelX, setObjectVelX));
        v.push_back(v8helpers::ParamContainer("velY", getObjectVelY, setObjectVelY));
    }
	return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

/**
 * Implementations of static setter and getter functions
 *
 * IMPORTANT: These setter and getter functions will set and get values of v8
 * callback data structures. Note their return type is void regardless of
 * whether they are setter or getter. 
 *
 * Also keep in mind that the function signature must match this exactly in
 * order for v8 to accept these functions. 
 */ 

void Object::setObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Object*>(ptr)->setPosition(value->Int32Value(), static_cast<Object*>(ptr)->getPosition().y);
}

void Object::getObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<Object*>(ptr)->getPosition().x;
	info.GetReturnValue().Set(x_val);
}

void Object::setObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Object*>(ptr)->setPosition(static_cast<Object*>(ptr)->getPosition().x, value->Int32Value());
}

void Object::getObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int y_val = static_cast<Object*>(ptr)->getPosition().y;
	info.GetReturnValue().Set(y_val);
}

void Object::setObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	static_cast<Object*>(ptr)->guid = *utf8_str;
}

void Object::getObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string guid = static_cast<Object*>(ptr)->guid;
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str(), v8::String::kNormalString);
	info.GetReturnValue().Set(v8_guid);
}

void Object::setObjectCurrGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	static_cast<Object*>(ptr)->current_guid = *utf8_str;
}

void Object::getObjectCurrGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string current_guid = static_cast<Object*>(ptr)->current_guid;
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), current_guid.c_str(), v8::String::kNormalString);
	info.GetReturnValue().Set(v8_guid);
}

void Object::setObjectVelX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Object*>(ptr)->body->velocity.x = value->Int32Value();
}

void Object::getObjectVelX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<Object*>(ptr)->body->velocity.x;
	info.GetReturnValue().Set(x_val);
}

void Object::setObjectVelY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Object*>(ptr)->body->velocity.y = value->Int32Value();
}

void Object::getObjectVelY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int y_val = static_cast<Object*>(ptr)->body->velocity.y;
	info.GetReturnValue().Set(y_val);
}