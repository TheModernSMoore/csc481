#include "characterCollision.h"

using json = nlohmann::json;

CharacterCollision::CharacterCollision(Character *character_collided, Object *collided_with_character) {
    character = character_collided;
    collided_with = collided_with_character;
}

json CharacterCollision::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    output["ObjectID"] = collided_with->identifier;
    return output;
}