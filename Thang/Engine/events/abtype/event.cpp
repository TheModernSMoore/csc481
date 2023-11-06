#include "event.h"

using json = nlohmann::json;

Event::Event() {}

Event::~Event() {}

Event::getType() {
    return type;
}

json Event::clientJSONHelper() {
    json output;
    output["Type"] = type;
    return output;
}