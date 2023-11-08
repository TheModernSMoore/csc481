#include "event.h"

using json = nlohmann::json;

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