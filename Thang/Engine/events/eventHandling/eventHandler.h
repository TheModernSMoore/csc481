#pragma once
#include "../abtype/event.h"

class EventHandler
{
    public:
        // Constructors and Destructors
        EventHandler();
        virtual ~EventHandler() = 0;
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};