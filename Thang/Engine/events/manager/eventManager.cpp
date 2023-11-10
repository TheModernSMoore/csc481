#include "eventManager.h"

std::mutex EventManager::mutex;

EventManager* EventManager::inst = NULL;

std::map<EventType, std::list<EventHandler *>> EventManager::handlers;

std::priority_queue<Event*, std::vector<Event*>, std::greater<Event*>>  EventManager::raised_events;

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
    int64_t currTime = TimeManager::get()->getTimelines().at(0)->getCurrentTime();
    while (!raised_events.empty()) {
        Event *current = raised_events.pop();
        // If the next event is supposed to happen in the future, don't handle it
        if (current->getStamp() > currTime) {
            raised_events.push(current);
            break;
        }
        for (auto & handler : handlers.at(current->getType())) {
            handler->onEvent(current);
        }
    }
}