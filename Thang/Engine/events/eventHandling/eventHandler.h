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


class CharCollideHandler : public EventHandler
{
    public:
        // Constructors
        CharCollideHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};


class CharDeathHandler : public EventHandler
{
    public:
        // Constructors
        CharDeathHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};


class CharSpawnHandler : public EventHandler
{
    public:
        // Constructors
        CharSpawnHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};


class InputHandler : public EventHandler
{
    public:
        // Constructors
        InputHandler();
        // Function that actually handles the event
        virtual void onEvent(Event e) = 0;
};