#include "eventManager.h"

std::mutex EventManager::mutex;

EventManager* EventManager::inst = NULL;

std::map<EventType, std::list<EventHandler *>> EventManager::handlers;

std::vector<Event *> EventManager::raised_events;

EventManager::EventManager() {}

EventManager* EventManager::get() {
    if (inst == NULL) {
        inst = new EventManager();
    }
    return inst;
}

void EventManager::addEventToHandler(std::list<EventType> types, EventHandler *handler) {
    for (EventType & type : types) {
        handlers.at(type).push_back(handler);
    }
}

void EventManager::removeEventFromHandler(std::list<EventType> types, EventHandler *handler) {
    // for (EventType & type : types) {
    //     // Ion even know
    // }
}

void EventManager::raise(Event *e) {
    raised_events.push_back(e);
}

void EventManager::handleEvents() {
    // ITERATE THROUGH RAISED EVENTS, AND DECREMENT TIME TIL IN EVENT (WHY TIMESTAMP??)
}