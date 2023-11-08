#pragma once
#include "abtype/event.h"
#include "../objects/spawnPoint.h"
#include "../objects/character.h"
#include "../objects/manager/objectManager.h"



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
};