#include "charSpawnHandler.h"

CharSpawnHandler::CharSpawnHandler();

void CharSpawnHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_SPAWN) {
        CharacterSpawn *inp = (CharacterSpawn *) e;
        // Do event handling here
        // get character and just put its position to the
        // spawn point, then re-add it to the list of stuff
        // OR HAVE DEAD STATE ON CHARACTER CAUSE THAT MAY DO
        // SOME FINIKY STUFF WITH CLIENT
    }
}