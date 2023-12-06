#include "timeline.h"

#include "../v8helpers.h"


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

void Timeline::setTimePause(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    static_cast<Timeline*>(ptr)->paused = value->Int32Value();
}

void Timeline::getTimePause(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    int paused = static_cast<Timeline*>(ptr)->isPaused();
    info.GetReturnValue().Set(paused);
}

void Timeline::setDeltaTime(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    static_cast<Timeline*>(ptr)->delta_time = value->Int32Value();
}

void Timeline::getDeltaTime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    float delta_time = static_cast<Timeline*>(ptr)->delta_time;
    info.GetReturnValue().Set(delta_time);
}

v8::Local<v8::Object> Timeline::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("paused", getTimePause, setTimePause));
    v.push_back(v8helpers::ParamContainer("deltaTime", getDeltaTime, setDeltaTime));

	return v8helpers::exposeToV8("localTime", this, v, isolate, context, context_name);
}