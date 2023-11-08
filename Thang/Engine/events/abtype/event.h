#pragma once
#include "eventCommon.h"
#include <nlohmann/json.hpp>
#include "../../timeline/timeManager.h"

class Event
{
    protected:
        // The enumerable type of the event
        EventType type;
        // The way to help build json
        nlohmann::json clientJSONHelper();
        // Time until the event wants to be handled
        int64_t time_stamp;
    
    public:
        //Constructor and destructor below
        Event();
        virtual ~Event() = 0;
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON() = 0;

        EventType getType();

};