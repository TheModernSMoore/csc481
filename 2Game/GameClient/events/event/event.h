#pragma once
#include <nlohmann/json.hpp>
#include "../../timeline/timeManager.h"
#include "../../objects/manager/objectManager.h"
#include "eventType.h"

class Object;

class Character;

class SpawnPoint;

class Enemy;

class Event
{
    protected:
        // The enumerable type of the event
        EventType type;
        // The way to help build json
        nlohmann::json clientJSONHelper();
        // Time that the event wants to be handled in
        int64_t time_stamp;
    
    public:
        //Constructor and destructor below
        Event();
        virtual ~Event() = 0;
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON() = 0;

        EventType getType();

        int64_t getStamp();

        bool operator>(const Event& other) const;

};


class CharacterCollision : public Event
{
    private:
        // IMPORTANT TO CHECK THAT THIS STILL EXISTS SINCE ASYNCHRONOUSITY
        Object *collided_with;
        // Character who the collision will affect
        Character *character;

    public:
        // Constructor
        CharacterCollision(Character *character_collided, Object *collided_with_character);
        // To json function
        virtual nlohmann::json toClientJSON();
        // gets the object collided with the character
        Object* getOther();
        // gets the character
        Character* getCharacter();
};

class EnemyCollision : public Event
{
    private:
        // IMPORTANT TO CHECK THAT THIS STILL EXISTS SINCE ASYNCHRONOUSITY
        Object *collided_with;
        // Character who the collision will affect
        Enemy *enemy;

    public:
        // Constructor
        EnemyCollision(Enemy *enemy_collided, Object *collided_with_enemy);
        // To json function
        virtual nlohmann::json toClientJSON();
        // gets the object collided with the character
        Object* getOther();
        // gets the character
        Enemy* getEnemy();
};


class CharacterDeath : public Event
{
    private:
        // reference to the character being handled, check if still exists
        Character *character;

    public:
        // Constructor
        CharacterDeath(Character *character_to_die);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
        // gets the character
        Character* getCharacter();
};


class CharacterSpawn : public Event
{
    private:
        // reference to the character being handled, check if still exists
        Character *character;

    public:
        // Constructor
        CharacterSpawn(Character *character_to_spawn);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
        // gets the character
        Character* getCharacter();
        // gets the spawn point
        SpawnPoint* getSpawn();
};


class UserInput : public Event
{
    private:
        // Input string to parse and handle movement with
        std::string input_string;
        // Reference to character to have input affect
        Character *character;

    public:
        // Constructor that could calculate user input and store it here
        UserInput(Character *character_controlled, std::string input);
        // To json function
        virtual nlohmann::json toClientJSON();
        // gets the character
        Character* getCharacter();
        // gets the input string
        std::string getInputString();
};


class Pause : public Event
{
    public:
        Pause();
        // To json function
        virtual nlohmann::json toClientJSON();
};


class CycleSpeed : public Event
{
    public:
        CycleSpeed();
        // To json function
        virtual nlohmann::json toClientJSON();
};


class BulletShoot : public Event
{
    private:
        // reference to the character being handled, check if still exists
        Object *object;


    public:
        // Constructor
        BulletShoot(Object *object);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
        // gets the object that shot the bullet
        Object* getObject();
};