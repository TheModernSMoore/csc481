#include "userInput.h"

using json = nlohmann::json;

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