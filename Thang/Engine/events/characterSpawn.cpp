#include "characterSpawn.h"

using json = nlohmann::json;

CharacterSpawn::CharacterSpawn(Character *character_to_spawn, SpawnPoint *place_to_spawn) {
    character = character_to_spawn;
    spawnPoint = place_to_spawn;
}

json CharacterSpawn::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    output["SpawnPointID"] = spawnPoint->identifier;
    return output;
}