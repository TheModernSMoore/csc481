#pragma once
#include <vector>
#include "timeline.h"

class TimeManager
{
    private:
        /// the static list of game objects
        static std::vector<Timeline*> timelines;
        //The single instance of the TimeManager
        static TimeManager* inst;

        TimeManager();
    
    public:
        static TimeManager* get();
        // adds a game object to the objects vector
        void addTimeline(Timeline *timeline);
        // removes a game object from the objects vector
        //void removeTimeline(Timeline *timeline);
        // gets the objects vector (used in main for updating, could delegate it to here for [insert smart terminology here])
        std::vector<Timeline*> getTimelines();

};