#include "charCollideHandler.h"
#include "../characterCollision.h"

CharCollideHandler::CharCollideHandler();

void CharCollideHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_COLLISION) {
        CharacterCollision *charc = (CharacterCollision *) e;
        // Do Event handling
        // get character and object, see what object it is, then throw new events
        // (character death throw)
        if (charc->collided_with->getObjectType().compare("DeathBox") == 0) {
            // Find spawn point closes to character on the left
            ObjectManager objectManager = ObjectManager::get();
            float shortest_dist = std::numeric_limits<float>::max();
            SpawnPoint *shortest_to = nullptr;
            for (auto & [key, object] : objectManager->getObjects()) {
                if (object->getObjectType().compare("SpawnPoint") == 0) {
                    float distance = charc->character->getPosition().x - object->getPosition().x;
                    if (distance >= 0 && distance < shortest_dist) {
                        shortest_dist = distance;
                        shortest_to = (SpawnPoint *) object;
                    }
                }
            }
            // RAISE SPAWN EVENT 
        }
    }
}