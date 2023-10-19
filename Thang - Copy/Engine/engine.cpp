#include <SFML/Window.hpp>
#include "objects/platform.h"
#include "objects/character.h"
#include "timeline/timeManager.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

void clientMessaging(int client_number, std::vector<std::string> *inputs, std::mutex *m, std::vector<Character*> *characters)
{
    // std::unique_lock<std::mutex> initLock(*m);
    ObjectManager *objectManager = ObjectManager::get();
    // HW3 TYPE BEAT CREATE CHARACTER TO SPAWN IN
    // Character man(20, 5, 0.5, 10, 0.3);
    // man.setPosition(400.f, 150.f);
    
    // objectManager->addObject(&man);
    // characters->push_back(&man);

    zmq::context_t comtext(1);
    zmq::socket_t comsock(comtext, zmq::socket_type::rep);
    std::string endpoint("tcp://localhost:" + std::to_string(5555 + client_number)); // find next available local host connection
    comsock.bind(endpoint.c_str());
    std::cout << "binding to " << endpoint << std::endl;
    while (1) {
        zmq::message_t input_message;
        // std::vector<Character> real_characters;
        // std::vector<Platform> real_platforms;
        auto res = comsock.recv(input_message);
        {
            // Might be able to put all this in a seperate PUB/SUB socket just so each client isn't asking for this each time
            std::unique_lock<std::mutex> lock(*m);
            inputs->at(client_number - 1) = input_message.to_string();
            std::vector<Object*> objects = objectManager->getObjects();
            int size = objects.size();
            int current = 0;
            for (auto object : objects) {
                current++;
                comsock.send(zmq::buffer(object->toClientString()), current < size ? zmq::send_flags::sndmore : zmq::send_flags::none);
            }
        }
    }
}

void clientCreation(int *clients_connected, std::vector<std::string> *inputs, std::mutex *m, std::vector<Character*> *characters)
{
    // (construct the socket for that as well)
    std::vector<std::thread> life_extender;  
    zmq::context_t connecxt(1);
    zmq::socket_t connect_checker(connecxt, zmq::socket_type::rep);
    connect_checker.bind("tcp://localhost:5555");
    while (1) {
        zmq::message_t connect_req;
        auto res = connect_checker.recv(connect_req);
        {
            std::unique_lock<std::mutex> lock(*m);
            (*clients_connected) ++;
            inputs->push_back(std::string("")); // this will be at clients_connected - 1
        }
        // MUST EXTEND THIS THREADS LIFE
        life_extender.push_back(std::thread(clientMessaging, *clients_connected, inputs, m, characters));
        connect_checker.send(zmq::buffer(std::to_string(*clients_connected)));
    }
}

int main(int argc, char const *argv[])
{
    TimeManager *timeManager = TimeManager::get();
    Timeline realTime = Timeline();
    Timeline localTime = Timeline(&realTime, 65000);
    timeManager->addTimeline(&realTime);
    timeManager->addTimeline(&localTime);


    ObjectManager *objectManager = ObjectManager::get();

    Platform morb(sf::Vector2f(120.f, 25.f));
    morb.setPosition(215.f, 250.f);
    morb.setSpeed(0, 2);
    morb.setRange(0, 200);
    morb.setFillColor(sf::Color(166, 126, 18));

    objectManager->addObject(&morb);

    Platform borb(sf::Vector2f(120.f, 25.f));
    borb.setPosition(375.f, 450.f);
    borb.setSpeed(2, 0);
    borb.setRange(200, 0);
    borb.setFillColor(sf::Color(166, 126, 18));

    objectManager->addObject(&borb);

    Platform ground(sf::Vector2f(800.f, 25.f));
    ground.setPosition(0, 600 - 25);
    sf::Texture texture;
    if (!texture.loadFromFile("img/grass.jpg"))
        return -1;
 
    ground.setTexture(&texture);
    

    objectManager->addObject(&ground);


    Platform wall(sf::Vector2f(25.f, 600-25));
    wall.setPosition(100, 0);

    objectManager->addObject(&wall);

    Platform wall2(sf::Vector2f(25.f, 600-25));
    wall2.setPosition(700, 0);
    
    objectManager->addObject(&wall2);

    std::vector<Character*> characters;

    Character man1(20, 5, 0.5, 10, 0.3);
    man1.setPosition(200.f, 150.f);
    
    objectManager->addObject(&man1);
    characters.push_back(&man1);

    Character man2(20, 5, 0.5, 10, 0.3);
    man2.setPosition(400.f, 150.f);
    
    objectManager->addObject(&man2);
    characters.push_back(&man2);

    Character man3(20, 5, 0.5, 10, 0.3);
    man3.setPosition(600.f, 150.f);
    
    objectManager->addObject(&man3);
    characters.push_back(&man3);


    std::mutex m;


    std::vector<std::string> inputs;
    int clients_connected = 0;

    std::thread client_creator(clientCreation, &clients_connected, &inputs, &m, &characters);

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
                if(!(localTime.isPaused()))
                    characters.at(idx++)->input(input);
            }
            if(!(localTime.isPaused()))
                objectManager->updateObjects();
        }

    }
    return 0;
}
