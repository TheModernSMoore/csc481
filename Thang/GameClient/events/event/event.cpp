#include "event.h"

using json = nlohmann::json;

// Abstract Event implementation

Event::Event() {
    this->time_stamp = TimeManager::get()->getTimelines().at(0)->getCurrentTime();
}

Event::~Event() {}

bool Event::operator>(const Event& other) const {
    return this->time_stamp > other.time_stamp;
}

EventType Event::getType() {
    return type;
}

int64_t Event::getStamp() {
    return time_stamp;
}

json Event::clientJSONHelper() {
    json output;
    output["Type"] = type;
    return output;
}

// Character collision implementation

CharacterCollision::CharacterCollision(Character *character_collided, Object *collided_with_character) {
    type = CHARACTER_COLLISION;
    character = character_collided;
    collided_with = collided_with_character;
}

json CharacterCollision::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    output["ObjectID"] = collided_with->identifier;
    return output;
}

Object* CharacterCollision::getOther() {
    return collided_with;
}

Character* CharacterCollision::getCharacter() {
    return character;
}

// Character death implementation

CharacterDeath::CharacterDeath(Character *character_to_die) {
    type = CHARACTER_DEATH;
    character = character_to_die;
    
}

json CharacterDeath::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    return output;
}

Character* CharacterDeath::getCharacter() {
    return character;
}

// Character spawn implementation

CharacterSpawn::CharacterSpawn(Character *character_to_spawn, SpawnPoint *place_to_spawn) {
    type = CHARACTER_SPAWN;
    character = character_to_spawn;
    spawnPoint = place_to_spawn;
    time_stamp += 3 * 1000000;
}

json CharacterSpawn::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    output["SpawnPointID"] = spawnPoint->identifier;
    return output;
}

Character* CharacterSpawn::getCharacter() {
    return character;
}

SpawnPoint* CharacterSpawn::getSpawn() {
    return spawnPoint;
}

// User Input implementation

UserInput::UserInput(Character *character_controlled, std::string input) {
    type = USER_INPUT;
    character = character_controlled;
    input_string = input;
}

json UserInput::toClientJSON() {
    json output = clientJSONHelper();
    output["Input"] = input_string;
    output["CharacterID"] = character->identifier;
    return output;
}

Character* UserInput::getCharacter() {
    return character;
}

std::string UserInput::getInputString() {
    return input_string;
}

// Pause Event implementation

Pause::Pause() {
    type = PAUSE;
}

json Pause::toClientJSON() {
    return clientJSONHelper();
}

// Cycle Speed Event implementation

CycleSpeed::CycleSpeed() {
    type = CYCLE_SPEED;
}

json CycleSpeed::toClientJSON() {
    return clientJSONHelper();
}