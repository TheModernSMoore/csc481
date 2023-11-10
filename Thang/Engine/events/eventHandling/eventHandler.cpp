#include "eventHandler.h"

EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

// Character Collider Handler implementation

CharCollideHandler::CharCollideHandler() {}

void CharCollideHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_COLLISION) {
        CharacterCollision *charc = (CharacterCollision *) e;
        // Do Event handling
        // get character and object, see what object it is, then throw new events
        if (charc->getOther()->getObjectType().compare("DeathBox") == 0) {
            // Raise death event
            EventManager::get()->raise(new CharacterDeath(charc->getCharacter()));
        }
    }
}

// Character Death Handler implementation

CharDeathHandler::CharDeathHandler() {}

void CharDeathHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_DEATH) {
        CharacterDeath *dea = (CharacterDeath *) e;
        // Do input handling here
        // get character and remove it from the list,
        // (maybe before above) find nearest spawn point and
        // throw all that into a spawn event
        // Find spawn point closes to character on the left
        ObjectManager objectManager = ObjectManager::get();
        float shortest_dist = std::numeric_limits<float>::max();
        SpawnPoint *shortest_to = nullptr;
        for (auto & [key, object] : objectManager->getObjects()) {
            if (object->getObjectType().compare("SpawnPoint") == 0) {
                float distance = dea->getCharacter()->getPosition().x - object->getPosition().x;
                if (distance >= 0 && distance < shortest_dist) {
                    shortest_dist = distance;
                    shortest_to = (SpawnPoint *) object;
                }
            }
        }
        ObjectManager::get()->removeObject(dea->getCharacter());
        EventManager::get()->raise(new CharacterSpawn(dea->getCharacter(), shortest_to));
    }
}

// Character Spawn Handler implementation

CharSpawnHandler::CharSpawnHandler() {}

void CharSpawnHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_SPAWN) {
        CharacterSpawn *spa = (CharacterSpawn *) e;
        // Do event handling here
        // get character and just put its position to the
        // spawn point, then re-add it to the list of stuff
        // OR HAVE DEAD STATE ON CHARACTER CAUSE THAT MAY DO
        // SOME FINIKY STUFF WITH CLIENT

        ObjectManager::get()->addObject(dea->getCharacter());
        spa->getCharacter()->setPosition(spa->getSpawn()->getPosition());
    }
}

// User Input Handler implementation

InputHandler::InputHandler() {}

void InputHandler::onEvent(Event *e) {
    if (e->getType() = USER_INPUT) {
        UserInput *inp = (UserInput *) e;
        // Do input handling here
        // get character, and maybe not move character
        // just update it's desired direction?
    }
}