#pragma once
#include "../eventHandling/eventHandler.h"
#include "../event/event.h"
#include "../../objects/manager/objectManager.h"
#include <list>
#include <queue>

class EventHandler;

class EventManager
{
    private:
        // Private constuctor
        EventManager();
        // Singular instance of event manager
        static EventManager *inst;
        // Map of key: types, and value: list of handlers to handle those types
        static std::map<EventType, std::list<EventHandler *>> handlers;
        // Vector of currently raised events waiting to be handled
        // JUST ITERATE THORUGH AND LOWER PRIORITY AND IF < 0 HANDLE IT IF NOT DON'T LEL
        static std::priority_queue<Event*, std::vector<Event*>, std::greater<Event*>>  raised_events;
    
    public:
        // gets the single instance of the Manager
        static EventManager* get();
        // Adds an event handler with the types of events
        void addEventToHandler(std::list<EventType> types, EventHandler *handler);
        // Removes an event handler from the given event types
        void removeEventFromHandler(std::list<EventType> types, EventHandler *handler);
        // Raises an event to be queued up
        void raise(Event *e);
        // Raises an event from a json
        void raise(nlohmann::json jevent);
        // This will get called every update cycle
        void handleEvents();
        // This raises an event from scripts
        static void scriptedRaise(const v8::FunctionCallbackInfo<v8::Value>& args);
};