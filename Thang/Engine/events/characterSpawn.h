#pragma once
#include "abtype/event.h"
#include "../objects/spawnPoint.h"



class CharacterSpawn : public Event
{
    private:
        // Spawn Point for the player to be spawned at
        SpawnPoint *spawnPoint;
        // reference to the character being handled, check if still exists
        Character *character;

    public:
        // Constructor
        CharacterSpawn(Character *character_to_spawn, SpawnPoint *place_to_spawn);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON();
}