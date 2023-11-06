#include "charCollideHandler.h"
#include "../characterCollision.h"

CharCollideHandler::CharCollideHandler();

void CharCollideHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_COLLISION) {
        CharacterCollision *inp = (CharacterCollision *) e;
        // Do input handling here
        // get character and object, see what object it is, then throw new events
        // (character death throw)
    }
}