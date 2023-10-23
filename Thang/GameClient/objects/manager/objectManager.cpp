#include "objectManager.h"
#include <cstdio>

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

ObjectManager* ObjectManager::inst = NULL;


ObjectManager::ObjectManager() {}

ObjectManager* ObjectManager::get() {
    if (inst == NULL) {
        inst = new ObjectManager();
    }
    return inst;
}

std::map<int, Object*> ObjectManager::objects;

std::map<int, Object*> ObjectManager::bodies;

std::map<int, Object*> ObjectManager::visibles;

void ObjectManager::addObject(Object *object)
{
    objects.emplace(std::make_pair(object->identifier, object));
    if (object->body) {
        bodies.emplace(std::make_pair(object->identifier, object));
    }
    if (object->visible) {
        visibles.emplace(std::make_pair(object->identifier, object));
    }
}

void ObjectManager::removeObject(Object *object)
{
    // objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
    // if (object->body) {
    //     bodies.erase(std::remove(bodies.begin(), bodies.end(), object), bodies.end());
    // } 
    // // else if (object->visible) {
    // //     visibles.erase(std::remove(visibles.begin(), visibles.end(), object), visibles.end());
    // // }
}

std::map<int, Object*> ObjectManager::getObjects()
{
    return objects;
}

std::map<int, Object*> ObjectManager::getVisibles()
{
    return visibles;
}

Object* ObjectManager::parseObjJSON(nlohmann::json to_parse)
{  // READ JSON, IF IDENTIFIER EXISTS IN OBJ LIST, JUST UPDATE POSITION (or any other necessary componets)
// ELSE CREATE OBJ WITH IDENTIFIER IN OBJECTS
    auto iterator_thing = objects.find(to_parse["Identifier"]);
    Object *parsed = nullptr;
    if (objects.find(to_parse["Identifier"]) == objects.end()) {
        std::string type = to_parse["Type"];
        if (type.compare("Platform") == 0) {
            // Construct Platform
            sf::Vector2f size(to_parse["Size"][0], to_parse["Size"][1]);
            Platform platform(size);
            platform.identifier = to_parse["Identifier"];
            sf::Color color(to_parse["Color"][0], to_parse["Color"][1], to_parse["Color"][2]);
            platform.setFillColor(color);
            sf::Texture texture;
            texture.loadFromFile(to_parse["Texture"]);

            parsed = &platform;
            addObject(parsed);

        } else if (type.compare("Character") == 0) {
            // Construct Character
            Character character(to_parse["Radius"], to_parse["Speed"], to_parse["Accel"], to_parse["JumpSpeed"]);
            character.identifier = to_parse["Identifier"];
            sf::Color color(to_parse["Color"][0], to_parse["Color"][1], to_parse["Color"][2]);
            character.setFillColor(color);
            sf::Texture texture;
            texture.loadFromFile(to_parse["Texture"]);

            parsed = &character;
            addObject(parsed);
        } else {
            // DO NOTHING FOR NOW IDK IF CHANGE OR WHA
        }
    } else {
        parsed = iterator_thing->second;
    }
    float x = to_parse["Position"][0];
    float y = to_parse["Position"][1];
    parsed->setPosition(x, y);
    return parsed;
    
}

// wrapper function for the threads that we create below
void ObjectManager::physicsLogicWrapper()
{
    for(auto & [key, physics] : bodies) {
        physics->logic();//                       TODO CHANGE THIS
    }
}

// wrapper function for the threads that we create below
void ObjectManager::genericLogicWrapper()
{
    // for(auto & generic : generics) {
    //     generic->logic();                         TODO CHANGE THIS
    // }
}

void ObjectManager::updateObjects()
{
    std::vector<std::thread> threads;
    // threading the logic of the objects
    threads.push_back(std::thread(physicsLogicWrapper));
    threads.push_back(std::thread(genericLogicWrapper));
    for (auto & thread : threads) {
        thread.join();
    }
}

std::vector<Object*> ObjectManager::overlapped(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isOverlapped(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

//The five below functions just find all of the objects that are touching a certain object
//Utilizing the isTouching function in Object

std::vector<Object*> ObjectManager::touching(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isTouching(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBelow(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isTouchingBelow(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingRight(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isTouchingRight(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingLeft(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isTouchingLeft(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingAbove(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : objects)
    {
        if (other != object) {
            if (object->isTouchingAbove(other)) {
                touchers.push_back(other);
            }
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::overlappedPhysics(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isOverlapped(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

//The five below functions just find all of the physicsAffect objects that are touching a certain object
//Utilizing the isOverlapped function in Object
std::vector<Object*> ObjectManager::touchingPhysics(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isTouching(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingPhysicsBelow(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isTouchingBelow(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingPhysicsRight(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isTouchingRight(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingPhysicsLeft(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isTouchingLeft(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingPhysicsAbove(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->body->physicsAffected && object->isTouchingAbove(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::overlappedBodies(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isOverlapped(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

//The five below functions just find all of the physicsAffect objects that are touching a certain object
//Utilizing the isOverlapped function in Object
std::vector<Object*> ObjectManager::touchingBodies(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isTouching(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBodiesBelow(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isTouchingBelow(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBodiesRight(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isTouchingRight(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBodiesLeft(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isTouchingLeft(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

std::vector<Object*> ObjectManager::touchingBodiesAbove(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && object->isTouchingAbove(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}