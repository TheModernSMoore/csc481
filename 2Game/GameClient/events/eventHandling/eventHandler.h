#pragma once
#include "../manager/eventManager.h"
#include "../event/event.h"
#include "../event/eventType.h"

class Event;

class EventHandler
{
    public:
        // Constructors and Destructors
        EventHandler();
        virtual ~EventHandler() = 0;
        // Function that actually handles the event
        virtual void onEvent(Event *e) = 0;
};


class CharCollideHandler : public EventHandler
{
    public:
        // Constructors
        CharCollideHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};

class EnCollideHandler : public EventHandler
{
    public:
        // Constructors
        EnCollideHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};


class CharDeathHandler : public EventHandler
{
    public:
        // Constructors
        CharDeathHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};


class CharSpawnHandler : public EventHandler
{
    public:
        // Constructors
        CharSpawnHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};


class InputHandler : public EventHandler
{
    public:
        // Constructors
        InputHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};


class PauseHandler : public EventHandler
{
    public:
        // Constructors
        PauseHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};

class SpeedHandler : public EventHandler
{
    public:
        // Constructors
        SpeedHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};

class BulletHandler : public EventHandler
{
    public:
        // Constructors
        BulletHandler();
        // Function that actually handles the event
        virtual void onEvent(Event *e);
};