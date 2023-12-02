#pragma once
#include <limits>
#include <mutex>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "../common.h"
#include <nlohmann/json.hpp>
#include <cmath>
#include <iostream>
#include <utility>
#include <v8.h>
// #include "../../events/manager/eventManager.h"

#define DOWN_ACCEL 0.3
#define TERM_VEL -6

/// The generic object class that is the base of all game objects
class Object : public sf::Shape
{
    friend class objectManager;
    private:
        // Keeps track of how many drawable objects have been made (those sent to client) in order to make unique identifier
        static int objects_made;
        // mutex for objects_made increment and storing as identifiers
        static std::mutex createtex;

        v8::Isolate* isolate;
		v8::Global<v8::Context>* context;

        /**
		 * NOTE: These "Accessors" have to be **static**
		 *
		 * You will need to implement a setter and getter for every class
		 * member variable you want accessible to javascript.
		 */
		static void setObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
		static void setObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
		static void setObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
		static void getObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

    protected:
        // the mutex used for safe movement in all objects, hence protected and static
        static std::mutex movetex;

        void setBody(bool physicsAffected);

        void setVisible();

        void setCollisionArea();

        std::string object_type;

        // Could be in visible struct as well as color
        std::string tex_path;

        nlohmann::json clientJSONHelper();

    public:
        int identifier;

        std::string guid;

        object_body *body = nullptr;

        object_visible *visible = nullptr;

        object_collision_area *collision_area = nullptr;


        //Constructor and destructor below
        Object();
        virtual ~Object() = 0;
        //checks if objects are overlapped
        bool isOverlapped(Object *other);
        //returns smallest distance between this and other
        sf::Vector2f distanceBetween(Object *other);
        //checks if the distance between is 0
        bool isTouching(Object *other);
        //Checks if other is below this and touching.
        bool isTouchingBelow(Object *other);
        //Checks if other is to the left of this and touching.
        bool isTouchingLeft(Object *other);
        //Checks if other is to the left of this and touching.
        bool isTouchingRight(Object *other);
        //Checks if other is above this and touching.
        bool isTouchingAbove(Object *other);
        // Makes json info to be used for sending to client
        virtual nlohmann::json toClientJSON() = 0;
        //abstract function logic that all objects will have (all stuff that happens before draw)
        virtual void logic() = 0;
        // Overwrites move and does different behavior based off of components
        virtual void move(float offsetX, float offsetY);
        // 
        void setTexturePath(std::string texture_path);
        // returns object type string
        std::string getObjectType();

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