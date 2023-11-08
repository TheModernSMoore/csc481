#pragma once
#include "eventHandler.h"
#include "../characterDeath.h"

class CharDeathHandler : public EventHandler
{
    public:
        // Constructors
        CharDeathHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};