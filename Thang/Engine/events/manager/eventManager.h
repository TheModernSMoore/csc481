#pragma once
#include "../eventHandling/eventHandler.h"


class EventManager
{
    private:
        // Private constuctor
        EventManager();
        // Map of key: types, and value: list of handlers to handle those types
        std::map<EventType, std::list<EventHandler *>> handlers;
        // Vector of currently raised events waiting to be handled
        std::vector<Event *> raised_events;
    
    public:
        // Mutex to ensure events are handled appropriately
        static std::mutex mutex;
        // gets the single instance of the Manager
        static EventManager get();
        // Adds an event handler with the types of events
        void addHandler(std::list<EventType> types, EventHandler *handler);
        // Removes an event handler from the given event types
        void removeHandler(std::list<EventType> types, EventHandler *handler);
        // Raises an event to be queued up
        void raise(Event *e);
}