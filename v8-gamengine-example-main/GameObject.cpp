#include "GameObject.h"
#include <iostream>

#include <filesystem>
#include <cstring>
#include "ScriptManager.h"
#include "v8helpers.h"

/** Definitions of static class members */
int GameObject::current_guid = 0;
std::vector<GameObject*> GameObject::game_objects;

/**
 * Initialize position to (0,0); set guid to incrementing value based on
 * number of objects
 */
GameObject::GameObject() : x(0), y(0)
{
	guid = "gameobject" + std::to_string(current_guid);
	current_guid++;
	game_objects.push_back(this);
}

GameObject::~GameObject()
{
	context->Reset();
}

/**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work. 
 */
v8::Local<v8::Object> GameObject::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("x", getGameObjectX, setGameObjectX));
	v.push_back(v8helpers::ParamContainer("y", getGameObjectY, setGameObjectY));
	v.push_back(v8helpers::ParamContainer("guid", getGameObjectGUID, setGameObjectGUID));
	return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

/**
 * Implementations of static setter and getter functions
 *
 * IMPORTANT: These setter and getter functions will set and get values of v8
 * callback data structures. Note their return type is void regardless of
 * whether they are setter or getter. 
 *
 * Also keep in mind that the function signature must match this exactly in
 * order for v8 to accept these functions. 
 */ 

void GameObject::setGameObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<GameObject*>(ptr)->x = value->Int32Value();
}

void GameObject::getGameObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<GameObject*>(ptr)->x;
	info.GetReturnValue().Set(x_val);
}


void GameObject::setGameObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<GameObject*>(ptr)->y = value->Int32Value();
}

void GameObject::getGameObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int y_val = static_cast<GameObject*>(ptr)->y;
	info.GetReturnValue().Set(y_val);
}

void GameObject::setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	static_cast<GameObject*>(ptr)->guid = *utf8_str;
}

void GameObject::getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string guid = static_cast<GameObject*>(ptr)->guid;
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str(), v8::String::kNormalString);
	info.GetReturnValue().Set(v8_guid);
}

/**
 * Factory method for allowing javascript to create instances of native game
 * objects
 *
 * NOTE: Like with the setters above, this static function does have a return
 * type (and object), but the return value is placed in the function callback
 * parameter, not the native c++ return type.
 */
void GameObject::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	std::string context_name("default");
	if(args.Length() == 1)
	{
		v8::String::Utf8Value str(args.GetIsolate(), args[0]);
		context_name = std::string(v8helpers::ToCString(str));
#if GO_DEBUG
		std::cout << "Created new object in context " << context_name << std::endl;
#endif
	}
	GameObject *new_object = new GameObject();
	v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
	args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}

