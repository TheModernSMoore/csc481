#include "timeline.h"


Timeline::Timeline(Timeline *anchor, int64_t tic)
{
    this->anchor = anchor;
    this->tic = tic;
    start_time = anchor->getCurrentTime();
    current_time = start_time;
    last_time = current_time;
    delta_time = 0;
    for (int i = 0; i < 5; i++) {
        delta_times[i] = 0;
    }
    
}

Timeline::Timeline()
{
    this->tic = 1;
    anchor = nullptr;
    current_time = start_time;
    last_time = start_time;
    delta_time = 0;
    clock.restart();
}

void Timeline::setTime()
{
    last_time = current_time;
    if (anchor) {
        current_time = (anchor->getCurrentTime() - start_time) / tic;
        if (!paused) {
            delt_idx = (delt_idx + 1) % 5;
            delta_times[delt_idx] = current_time - last_time;
            int sum = 0;
            for (int i = 0; i < 5; i++)
            {
                sum += delta_times[i];
            }
            delta_time = sum;
        }
    } else {
        current_time = clock.getElapsedTime().asMicroseconds();
        delta_time = current_time - last_time;
    }
}

int64_t Timeline::getCurrentTime()
{
    return current_time;
}

int64_t Timeline::deltaTime()
{
    return delta_time;
}


void Timeline::pause()
{
    paused = true;
}

void Timeline::unpause()
{
    paused = false;
}

bool Timeline::isPaused()
{
    return paused;
}

void Timeline::cycleTic()
{
    if (time_mod_pos == 0) {
        time_mod_pos++;
        setTic(tic / 2); // 2
    } else if (time_mod_pos == 1) {
        time_mod_pos++;
        setTic(tic * 4); // 0.5
    } else if (time_mod_pos == 2) {
        time_mod_pos = 0;
        setTic(tic / 2); // 1
    }
}

void Timeline::setTic(int64_t newTic)
{
    for (int i = 0; i < 5; i++)
    {
        delta_times[i] = delta_times[i] * tic / newTic;
    }
    last_time = last_time * tic / newTic;
    current_time = current_time * tic / newTic;
    tic = newTic;
}