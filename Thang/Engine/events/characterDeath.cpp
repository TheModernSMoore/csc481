#include "characterDeath.h"

using json = nlohmann::json;

CharacterDeath::CharacterDeath(Character *character_to_die, ) {
    type = CHARACTER_DEATH;
    character = character_to_die;
    
}

json CharacterDeath::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    return output;
}