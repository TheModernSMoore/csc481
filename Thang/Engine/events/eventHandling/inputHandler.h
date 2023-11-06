#pragma once
#include "eventHandler.h"
#include "../userInput.h"

class InputHandler : public EventHandler
{
    public:
        // Constructors
        InputHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
}