#include <SFML/Window.hpp>
#include "objects/manager/objectManager.h"
#include "timeline/timeManager.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "events/manager/eventManager.h"

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
        auto res = comsock.recv(input_message, zmq::recv_flags::dontwait);
        if (res) {
            // Might be able to put all this in a seperate PUB/SUB socket just so each client isn't asking for this each time
            std::unique_lock<std::mutex> lock(*m);
            inputs->at(client_number - 1) = input_message.to_string();
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
    // sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window", sf::Style::Resize);
    // // change the position of the window (relatively to the desktop)
    // window.setPosition(sf::Vector2i(50, 50));


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

    Platform ground(sf::Vector2f(600.f, 25.f));
    ground.setPosition(0, 600 - 25);
    ground.setTexturePath("img/grass.jpg");
    

    objectManager->addObject(&ground);

    Platform ground_again(sf::Vector2f(550.f, 25.f));
    ground_again.setPosition(650, 600 - 25);
    ground_again.setTexturePath("img/grass.jpg");
    

    objectManager->addObject(&ground_again);


    Platform wall(sf::Vector2f(25.f, 600-25));
    wall.setPosition(100, 0);
    wall.setFillColor(sf::Color::Red);

    objectManager->addObject(&wall);

    SpawnPoint spawner;
    spawner.setPosition(200, 550);

    objectManager->addObject(&spawner);

    DeathBox pit(sf::Vector2f(1800.f, 50.f));
    pit.setPosition(0, 650);

    objectManager->addObject(&pit);

    std::vector<Character*> characters;


    std::mutex m;


    std::vector<std::string> inputs;
    int clients_connected = 0;

    std::thread client_creator(clientCreation, &clients_connected, &inputs, &m, &characters);


    EventManager *eventManager = EventManager::get();

    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_COLLISION}, new CharCollideHandler);
    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_DEATH}, new CharDeathHandler);
    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_SPAWN}, new CharSpawnHandler);


    // do not ask why this is necessary, IF I WERE TO CREATE 1 MORE OBJECT, THEY WOULD NOT APPEAR
    // AND THIS FIXED IT
    for (int i = 0; i < W; i++)  
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
                size_t place;
                if ((place = input.find("p")) != std::string::npos) {
                    swap_pause = true;
                    input.erase(place, 1);
                    localTime.isPaused() ? localTime.unpause() : localTime.pause();
                }
                if ((place = input.find("c")) != std::string::npos) {
                    cycle_tic = true;
                    input.erase(place, 1);
                    localTime.cycleTic();
                }
                if(!(localTime.isPaused())) {
                    characters.at(idx++)->input(input);
                }
            }
            if(!(localTime.isPaused())) {
                objectManager->updateObjects();
                eventManager->handleEvents();
            }

            // window.clear(sf::Color::Black);

            // for (auto & [key, value] : objectManager->getVisibles()) {
            //     window.draw(*value);
            // }
            // window.display();
        }
    }
    return 0;
}
