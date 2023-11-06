#pragma once
#include "eventCommon.h"
#include <nlohmann/json.hpp>

class Event
{
    protected:
        // The enumerable type of the event
        EventType type;
        // The way to help build json
        nlohmann::json clientJSONHelper();
        // age of the event (incremented every frame? or whenever other event handled?)
        int64_t age;
        // is it real-time?  higher priority if so
        bool real_time;
        // timestamp of event creation
        int64_t time_stamp;
    
    public:
        //Constructor and destructor below
        Event();
        virtual ~Event() = 0;
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON() = 0;

        EventType getType();

}