#include "charDeathHandler.h"

CharDeathHandler::CharDeathHandler();

void CharDeathHandler::onEvent(Event *e) {
    if (e->getType() = CHARACTER_DEATH) {
        CharacterDeath *inp = (CharacterDeath *) e;
        // Do input handling here
        // get character and remove it from the list,
        // (maybe before above) find nearest spawn point and
        // throw all that into a spawn event
    }
}