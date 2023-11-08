#pragma once
#include "abtype/event.h"


// This will most likely be raised on client but handled on server (with current implementation)
class UserInput : public Event
{
    private:
        // Input string to parse and handle movement with
        std::string input_string;
        // Reference to character to have input affect
        Character *character;

    public:
        // Constructor that could calculate user input and store it here
        UserInput(Character *character_controlled);
        // To json function
        virtual nlohmann::json toClientJSON();
};