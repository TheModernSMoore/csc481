#include "event.h"

using json = nlohmann::json;

// Abstract Event implementation

Event::Event() {
    this->time_stamp = TimeManager::get()->getTimelines().at(0)->getCurrentTime();
}

Event::~Event() {}

Event::getType() {
    return type;
}

json Event::clientJSONHelper() {
    json output;
    output["Type"] = type;
    return output;
}

// Character collision implementation

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

// Character death implementation

CharacterDeath::CharacterDeath(Character *character_to_die, ) {
    type = CHARACTER_DEATH;
    character = character_to_die;
    
}

json CharacterDeath::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    return output;
}

// Character spawn implementation

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

// User Input implementation

UserInput::UserInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        input_string += std::string("l");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        input_string += std::string("r");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        input_string += std::string("u");
    }
}

json UserInput::toClientJSON() {
    json output = clientJSONHelper();
    output["Input"] = input_string;
    output["CharacterID"] = character->identifier;
    return output;
}