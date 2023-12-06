#include "event.h"

#include "../../ScriptManager.h"
#include "../../v8helpers.h"

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

void Event::setType(EventType type) {
    this->type = type;
}

int64_t Event::getStamp() {
    return time_stamp;
}

json Event::clientJSONHelper() {
    json output;
    output["Type"] = type;
    return output;
}

void Event::setEventType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    static_cast<Event*>(ptr)->setType(static_cast<EventType>(value->Int32Value()));
}

void Event::getEventType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    int x_event_type = static_cast<Event*>(ptr)->getType();
    info.GetReturnValue().Set(x_event_type);
}

v8::Local<v8::Object> Event::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("type", getEventType, setEventType));
	return v8helpers::exposeToV8("event1", this, v, isolate, context, context_name);
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

CharacterSpawn::CharacterSpawn(Character *character_to_spawn) {
    type = CHARACTER_SPAWN;
    character = character_to_spawn;
    time_stamp += 3 * 1000000;
}

json CharacterSpawn::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    return output;
}

Character* CharacterSpawn::getCharacter() {
    return character;
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


// Color Event implementation

Color::Color(Character *color_to_change) {
    character = color_to_change;
    type = COLOR;
}

Character* Color::getCharacter() {
    return character;
}

json Color::toClientJSON() {
    json output = clientJSONHelper();
    output["CharacterID"] = character->identifier;
    return output;
}