#pragma once
#include "../eventHandling/eventHandler.h"


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
        static std::vector<Event *> raised_events;
    
    public:
        // Mutex to ensure events are handled appropriately
        static std::mutex mutex;
        // gets the single instance of the Manager
        static EventManager* get();
        // Adds an event handler with the types of events
        void addEventToHandler(std::list<EventType> types, EventHandler *handler);
        // Removes an event handler from the given event types
        void removeEventFromHandler(std::list<EventType> types, EventHandler *handler);
        // Raises an event to be queued up
        void raise(Event *e);
        // This will get called every update cycle
        void handleEvents();
};