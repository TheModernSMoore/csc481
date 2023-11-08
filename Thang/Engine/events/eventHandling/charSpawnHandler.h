#pragma once
#include "eventHandler.h"
#include "../characterSpawn.h"

class CharSpawnHandler : public EventHandler
{
    public:
        // Constructors
        CharSpawnHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};