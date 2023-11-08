#pragma once
#include "eventHandler.h"
#include "../characterCollision.h"

class CharCollideHandler : public EventHandler
{
    public:
        // Constructors
        CharCollideHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};