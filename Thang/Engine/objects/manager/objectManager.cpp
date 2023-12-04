#include "objectManager.h"
#include <cstdio>
#include <iostream>

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
    objects.erase(object->identifier);
    if (object->body) {
        bodies.erase(object->identifier);
    } 
    if (object->visible) {
        visibles.erase(object->identifier);
    }
}

std::map<int, Object*> ObjectManager::getObjects()
{
    return objects;
}

std::map<int, Object*> ObjectManager::getVisibles()
{
    return visibles;
}

std::map<int, Object*> ObjectManager::getBodies()
{
    return bodies;
}

int ObjectManager::parseObjJSON(nlohmann::json to_parse)
{  // READ JSON, IF IDENTIFIER EXISTS IN OBJ LIST, JUST UPDATE POSITION (or any other necessary componets)
// ELSE CREATE OBJ WITH IDENTIFIER IN OBJECTS
    auto iterator_thing = objects.find(to_parse["Identifier"]);
    Object *parsed = nullptr;
    std::string type = to_parse["Type"];
    if (objects.find(to_parse["Identifier"]) == objects.end()) {
        if (type.compare("Platform") == 0) {
            // Construct Platform
            sf::Vector2f size(to_parse["Size"][0], to_parse["Size"][1]);
            Platform *platform = new Platform(size);
            platform->identifier = to_parse["Identifier"];
            platform->setSpeed(to_parse["Speed"]);
            std::vector<sf::Vector2f> to_set;
            for (int i = 0; i < to_parse["PointAmount"]; i++) {
                to_set.push_back(sf::Vector2f(to_parse[std::string("Point") + std::to_string(i)][0], to_parse[std::string("Point") + std::to_string(i)][1]));
            }
            platform->setToGo(to_set);

            sf::Color color(to_parse["Color"][0], to_parse["Color"][1], to_parse["Color"][2]);
            platform->setFillColor(color);
            sf::Texture *texture = new sf::Texture;
            std::string tex_path(to_parse["Texture"]);
            if (tex_path.compare(std::string("")) !=0 && texture->loadFromFile(tex_path)) {
                platform->setTexture(texture);
            }

            parsed = platform;
            addObject(parsed);

        } else if (type.compare("Character") == 0) {
            // Construct Character
            Character *character = new Character(to_parse["Radius"], to_parse["Speed"], to_parse["Accel"], to_parse["JumpSpeed"]);
            character->identifier = to_parse["Identifier"];
            sf::Color color(to_parse["Color"][0], to_parse["Color"][1], to_parse["Color"][2]);
            character->setFillColor(color);
            sf::Texture *texture = new sf::Texture;
            std::string tex_path(to_parse["Texture"]);
            if (tex_path.compare(std::string("")) !=0 && texture->loadFromFile(tex_path)) {
                character->setTexture(texture);
            }

            parsed = character;
            addObject(parsed);
        } else if (type.compare("DeathBox") == 0) {
            // Construct DeathBox
            sf::Vector2f size(to_parse["Size"][0], to_parse["Size"][1]);
            DeathBox *deathBox = new DeathBox(size);

            parsed = deathBox;
            addObject(parsed);
        } else if (type.compare("SpawnPoint") == 0) {
            // Construct SpawnPoint
            SpawnPoint *spawnPoint = new SpawnPoint();

            parsed = spawnPoint;
            addObject(parsed);
        } else {
            return -1;
        }
    } else {
        parsed = iterator_thing->second;
    }
    if (type.compare("Platform") == 0) {
        dynamic_cast<Platform*>(parsed)->setNextPoint(to_parse["NextPoint"]);
    }
    float x = to_parse["Position"][0];
    float y = to_parse["Position"][1];
    parsed->setPosition(x, y);
    return parsed->identifier;
    
}

// wrapper function for the threads that we create below
void ObjectManager::physicsLogicWrapper()
{
    for(auto & [key, physics] : objects) {
        physics->logic();   //                   Seg faults explicitly on call to character->logic()
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
    for(auto & [key, physics] : objects) {
        physics->logic();   //                   Seg faults explicitly on call to character->logic()
    }
    // std::vector<std::thread> threads;
    // threading the logic of the objects
//     threads.push_back(std::thread(physicsLogicWrapper));
//     threads.push_back(std::thread(genericLogicWrapper));
//     for (auto & thread : threads) {
//         thread.join();
//     }
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
        if (other != object && other->body->physicsAffected && object->isOverlapped(other)) {
            touchers.push_back(other);
        }
    }
    return touchers;
}

void ObjectManager::areObjectsBelow(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() == 1 && args[0]->IsObject()) {
        v8::Local<v8::Object> obj_obj = args[0].As<v8::Object>();

        v8::External *external = v8::External::Cast(*obj_obj->GetInternalField(0));
        void* ptr = external->Value();
        Object* object = static_cast<Object*>(ptr);

        bool lean = ObjectManager::get()->touchingBelow(object).size() > 0;
        args.GetReturnValue().Set(lean);
    }
}

void ObjectManager::areObjectsAbove(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() == 1 && args[0]->IsObject()) {
        v8::Local<v8::Object> obj_obj = args[0].As<v8::Object>();

        v8::External *external = v8::External::Cast(*obj_obj->GetInternalField(0));
        void* ptr = external->Value();
        Object* object = static_cast<Object*>(ptr);

        bool lean = ObjectManager::get()->touchingAbove(object).size() > 0;
        args.GetReturnValue().Set(lean);
    }
}

//The five below functions just find all of the physicsAffect objects that are touching a certain object
//Utilizing the isOverlapped function in Object
std::vector<Object*> ObjectManager::touchingPhysics(Object *object)
{
    std::vector<Object*> touchers;
    for (auto & [key, other] : bodies)
    {
        if (other != object && other->body->physicsAffected && object->isTouching(other)) {
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
        if (other != object && other->body->physicsAffected && object->isTouchingBelow(other)) {
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
        if (other != object && other->body->physicsAffected && object->isTouchingRight(other)) {
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
        if (other != object && other->body->physicsAffected && object->isTouchingLeft(other)) {
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
        if (other != object && other->body->physicsAffected && object->isTouchingAbove(other)) {
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