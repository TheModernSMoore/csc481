#pragma once
#include <ctime>
#include <SFML/Window.hpp>
#include <SFML/System/Time.hpp>
#include <cstdint>
#include <v8.h>

// #define W 5

class Timeline
{
    private:
        // The start time of the Time class's initialization
        int64_t start_time;
        // the current time
        int64_t current_time;
        // the time of the last current time
        int64_t last_time;
        // units of each time step (1 by default to represent real time)
        int64_t tic = 1;
        // The position of the time modifier (0 = 1, 1 = 2, 2 = 0.5)
        int time_mod_pos = 0;
        // A flag to see if the game is paused (could maybe incorporate all pause functionality in pause function)
        bool paused = false;
        // Anchor is to another Time object linked to real time (if not null)
        Timeline *anchor;
        // Clock to gather real time from
        sf::Clock clock;
        // Array of delta-times to gather an average from
        int64_t delta_times[5];
        // The delta time to report
        int64_t delta_time;
        // index of delta time to replace
        int delt_idx = 0;

        static void setTimePause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getTimePause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
        static void setDeltaTime(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getDeltaTime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
        

    public:
        // Constructor taking in an anchor timeline and the tic size of this one
        Timeline(Timeline *anchor, int64_t tic);
        // Constructor of the base timeline
        Timeline();
        // Sets the current time of this timeline (in units designated by tic size) and changes last_time to the last current_time
        void setTime();
        // gets the current time of this timeline (in units designated by tic size)
        int64_t getCurrentTime();
        // The time between the last measured time and the current time
        int64_t deltaTime();
        // Pauses the game
        void pause();
        // Unpauses the game
        void unpause();
        // Checks if the game is paused
        bool isPaused();
        // Changes the tic size by 0.5, 1, or 2
        void cycleTic();
        // Sets the tic to the new tic and changes affected variables
        void setTic(int64_t newTic);

        /**
		 * This function will make this class instance accessible to scripts in
		 * the given context. 
		 *
		 * IMPORTANT: Please read this definition of this function in
		 * Object.cpp. The helper function I've provided expects certain
		 * parameters which you must use in order to take advance of this
		 * convinience. 
		 */
		v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name="default");



};