#pragma once
#include "abtype/event.h"
#include "../objects/character.h"



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
};