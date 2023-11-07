#include "timeManager.h"
#include <cstdio>

TimeManager* TimeManager::inst = NULL;


TimeManager::TimeManager() {}

TimeManager* TimeManager::get() {
    if (inst == NULL) {
        inst = new TimeManager();
    }
    return inst;
}

std::vector<Timeline*> TimeManager::timelines;

void TimeManager::addTimeline(Timeline *timeline)
{
    timelines.push_back(timeline);
}

// void TimeManager::removeTimeline(Timeline *timeline)
// {
//     timelines.erase(std::remove(timelines.begin(), timelines.end(), timeline), timelines.end());
// }

std::vector<Timeline*> TimeManager::getTimelines() {
    return timelines;
}