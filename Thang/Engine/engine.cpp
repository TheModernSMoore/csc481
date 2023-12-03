#include <SFML/Window.hpp>
#include "objects/manager/objectManager.h"
#include "timeline/timeManager.h"
#include "events/manager/eventManager.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <v8.h>
#include <libplatform/libplatform.h>
#include "ScriptManager.h"
#include "v8helpers.h"

using json = nlohmann::json;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

void clientMessaging(int client_number, std::vector<std::string> *inputs, std::mutex *m, std::vector<Character*> *characters)
{
    Character *man = new Character(20, 5, 0.5, 10);
    man->setPosition(400.f, 150.f);
    int character_ident = -1;
    ObjectManager *objectManager = ObjectManager::get();
    {
        std::unique_lock<std::mutex> initLock(*m);

        objectManager->addObject(man);
        characters->push_back(man);
        inputs->push_back(std::string("")); // this will be at clients_connected - 1
    }
    character_ident = man->identifier;


    TimeManager *timeManager = TimeManager::get();
    Timeline *globalTime = timeManager->getTimelines().at(0);
    zmq::context_t comtext(1);
    zmq::socket_t comsock(comtext, zmq::socket_type::rep);
    std::string endpoint("tcp://*:" + std::to_string(5555 + client_number)); // find next available local host connection
    comsock.bind(endpoint.c_str());
    std::cout << "binding to " << endpoint << std::endl;

    zmq::message_t idc_msg;
    auto res = comsock.recv(idc_msg);
    comsock.send(zmq::buffer(std::to_string(character_ident)));
    


    float time_since_recv = 0;
    while (1) {
        zmq::message_t input_message;
        // DO RECVMORE AND DONT WAIT SO WE CAN FIND AND CONSTRUCT EVENTS FROM CLIENT
        auto res = comsock.recv(input_message, zmq::recv_flags::dontwait);
        if (res) {
            // Might be able to put all this in a seperate PUB/SUB socket just so each client isn't asking for this each time
            std::unique_lock<std::mutex> lock(*m);
            json recved_json = json::parse(input_message.to_string());
            EventManager *eventManager = EventManager::get();
            if (recved_json["Type"] == PAUSE) {
                eventManager->raise(new Pause);
                comsock.send(zmq::buffer("ok"));
            } else if (recved_json["Type"] == CYCLE_SPEED) {
                eventManager->raise(new CycleSpeed);
                comsock.send(zmq::buffer("ok"));
            } else {
                inputs->at(client_number - 1) = recved_json["Input"];
                std::map<int, Object*> objects = objectManager->getObjects();
                int size = objects.size();
                int current = 0;
                for (auto pair : objects) {
                    Object* object = pair.second;
                    current++;
                    json client_json = object->toClientJSON();
                    std::string to_send = to_string(client_json);
                    comsock.send(zmq::buffer(to_send), current < size ? zmq::send_flags::sndmore : zmq::send_flags::none);
                }
            }
            time_since_recv = 0;
        } else {
            time_since_recv += (globalTime->deltaTime()) / 0.000001;
            if (time_since_recv >= 100000000000000) { // Ima be fr, idk why this number but it work
                break;
            }
        }
    }
    {
        std::unique_lock<std::mutex> lock(*m);
        objectManager->removeObject(man);
        inputs->at(client_number - 1) = std::string("");
    }
}

void clientCreation(int *clients_connected, std::vector<std::string> *inputs, std::mutex *m, std::vector<Character*> *characters)
{
    // (construct the socket for that as well)
    std::vector<std::thread> life_extender;  
    zmq::context_t connecxt(1);
    zmq::socket_t connect_checker(connecxt, zmq::socket_type::rep);
    connect_checker.bind("tcp://*:5555");
    while (1) {
        zmq::message_t connect_req;
        auto res = connect_checker.recv(connect_req);
        {
            std::unique_lock<std::mutex> lock(*m);
            (*clients_connected) ++;
        }
        // MUST EXTEND THIS THREADS LIFE
        life_extender.push_back(std::thread(clientMessaging, *clients_connected, inputs, m, characters));
        connect_checker.send(zmq::buffer(std::to_string(*clients_connected)));
    }
}

int main(int argc, char const *argv[])
{
    /*
     * NOTE: You must initialize v8 in main, otherwise handles will go out of scope.
     *
     * Contexts are generally on a per-thread basis, so if you would like to do 
     * script execution in a different thread, you must create a new context in
     * the other thread and give care to ensure the context doesn't go out of scope
     * before the thread's run function is called.
	 *
	 * The below v8 function calls are basically boilerplate. 
     */
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);

    { // anonymous scope for managing handle scope
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

		// Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));

		// // Bind the global static factory function for creating new GameObject instances
		// global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
		
        // Bind the global static function for retrieving object handles
		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));

        global->Set(isolate, "moveObject", v8::FunctionTemplate::New(isolate, Object::scriptMove));

        global->Set(isolate, "areObjectsBelow", v8::FunctionTemplate::New(isolate, Object::areObjectsBelow));

        global->Set(isolate, "areObjectsAbove", v8::FunctionTemplate::New(isolate, Object::areObjectsAbove));

        //                      global->Set(isolate, name of function inside of js, v8::Function Template::New(isolate, the function from c++ that will be under the given name))


        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

		// Without parameters, these calls are made to the default context

        //                                                These add scripts (.js files) so that they can be called from here (in C++)

        sm->addScript("hello_world", "scripts/hello_world.js");
        sm->addScript("character_logic", "scripts/character_logic.js");
        // sm->addScript("perform_function", "scripts/perform_function.js");

		// // Create a new context
		// v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
		// sm->addContext(isolate, object_context, "object_context");

		// // GameObject *go = new GameObject();
		// // go->exposeToV8(isolate, object_context);

		// With the "object_context" parameter, these scripts are put in a
		// different context than the prior three scripts
		// sm->addScript("create_object", "scripts/create_object.js", "object_context");
		// sm->addScript("random_object", "scripts/random_object.js", "object_context");
		// sm->addScript("random_position", "scripts/random_position.js", "object_context");
		// sm->addScript("modify_position", "scripts/modify_position.js", "object_context");

		// Use the following 4 lines in place of the above 4 lines as the
		// reference if you don't plan to use multiple contexts
		sm->addScript("create_object", "scripts/create_object.js");
		/* sm->addScript("random_object", "scripts/random_object.js"); */
		/* sm->addScript("random_position", "scripts/random_position.js"); */
		/* sm->addScript("modify_position", "scripts/modify_position.js"); */

        // sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window", sf::Style::Resize);
        // // change the position of the window (relatively to the desktop)
        // window.setPosition(sf::Vector2i(50, 50));

        std::cout << "jfas" << std::endl;


        TimeManager *timeManager = TimeManager::get();
        Timeline realTime = Timeline();
        Timeline localTime = Timeline(&realTime, 65000);
        timeManager->addTimeline(&realTime);
        timeManager->addTimeline(&localTime);


        ObjectManager *objectManager = ObjectManager::get();

        Platform morb(sf::Vector2f(120.f, 25.f));
        morb.setPosition(215.f, 250.f);
        std::vector<sf::Vector2f> morb_set;
        morb_set.push_back(sf::Vector2f(215.f, 450.f));
        morb_set.push_back(sf::Vector2f(215.f, 250.f));
        morb.setToGo(morb_set);
        morb.setSpeed(2);
        morb.setFillColor(sf::Color::Blue);

        objectManager->addObject(&morb);
        morb.exposeToV8(isolate, default_context);

        sm->runOne("create_object", false);

        Platform borb(sf::Vector2f(120.f, 25.f));
        borb.setPosition(375.f, 450.f);
        std::vector<sf::Vector2f> borb_set;
        borb_set.push_back(sf::Vector2f(575.f, 450.f));
        borb_set.push_back(sf::Vector2f(575.f, 350.f));
        borb_set.push_back(sf::Vector2f(375.f, 350.f));
        borb_set.push_back(sf::Vector2f(375.f, 450.f));
        borb.setToGo(borb_set);
        borb.setSpeed(2);
        borb.setFillColor(sf::Color(166, 126, 18));

        objectManager->addObject(&borb);
        borb.exposeToV8(isolate, default_context);

        sm->runOne("create_object", false);

        Platform ground(sf::Vector2f(600.f, 25.f));
        ground.setPosition(0, 600 - 25);
        ground.setTexturePath("img/grass.jpg");
        

        objectManager->addObject(&ground);
        ground.exposeToV8(isolate, default_context);

        sm->runOne("create_object", false);

        Platform ground_again(sf::Vector2f(550.f, 25.f));
        ground_again.setPosition(650, 600 - 25);
        ground_again.setTexturePath("img/grass.jpg");
        

        objectManager->addObject(&ground_again);
        ground_again.exposeToV8(isolate, default_context);


        Platform wall(sf::Vector2f(25.f, 600-25));
        wall.setPosition(100, 0);
        wall.setFillColor(sf::Color::Red);

        objectManager->addObject(&wall);
        wall.exposeToV8(isolate, default_context);

        SpawnPoint spawner;
        spawner.setPosition(200, 550);

        objectManager->addObject(&spawner);
        spawner.exposeToV8(isolate, default_context);

        SpawnPoint spawner2;
        spawner2.setPosition(800, 550);

        objectManager->addObject(&spawner2);
        spawner2.exposeToV8(isolate, default_context);

        DeathBox pit(sf::Vector2f(1800.f, 50.f));
        pit.setPosition(0, 650);

        objectManager->addObject(&pit);
        pit.exposeToV8(isolate, default_context);

        std::vector<Character*> characters;


        std::mutex m;


        std::vector<std::string> inputs;
        int clients_connected = 0;

        std::thread client_creator(clientCreation, &clients_connected, &inputs, &m, &characters);


        EventManager *eventManager = EventManager::get();

        std::cout << "jfas" << std::endl;

        eventManager->addEventToHandler(std::list<EventType> {CHARACTER_COLLISION}, new CharCollideHandler);
        eventManager->addEventToHandler(std::list<EventType> {CHARACTER_DEATH}, new CharDeathHandler);
        eventManager->addEventToHandler(std::list<EventType> {CHARACTER_SPAWN}, new CharSpawnHandler);
        eventManager->addEventToHandler(std::list<EventType> {USER_INPUT}, new InputHandler);
        eventManager->addEventToHandler(std::list<EventType> {PAUSE}, new PauseHandler);
        eventManager->addEventToHandler(std::list<EventType> {CYCLE_SPEED}, new SpeedHandler);
        
        // ScriptManager::get()->runOne("hello_world", false);

        std::cout << "bornana" << std::endl;


        // do not ask why this is necessary, IF I WERE TO CREATE 1 MORE OBJECT, THEY WOULD NOT APPEAR
        // AND THIS FIXED IT
        for (int i = 0; i < 5; i++)  
        {
            realTime.setTime();
            localTime.setTime();
        }

        while (1)
        {
            realTime.setTime();
            // set the new local time with each loop
            localTime.setTime();

            {
                std::unique_lock<std::mutex> lock(m);
                int idx = 0;
                bool swap_pause = false;
                bool cycle_tic = false;
                for (auto & input : inputs) {
                    if(!(localTime.isPaused())) {
                        eventManager->raise(new UserInput(characters.at(idx++), input));
                    }
                }
                eventManager->handleEvents();
                if(!(localTime.isPaused())) {
                    objectManager->updateObjects();
                }


                // window.clear(sf::Color::Black);

                // for (auto & [key, value] : objectManager->getObjects()) {
                //     window.draw(*value);
                // }
                // window.display();
            }
        }
    }

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();

    return 0;
}
