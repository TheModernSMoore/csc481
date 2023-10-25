#include <SFML/Window.hpp>
#include "objects/platform.h"
#include "objects/character.h"
#include "timeline/timeManager.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

using json = nlohmann::json;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

void clientMessaging(int client_number, std::vector<std::string> *inputs, std::mutex *m, std::vector<Character*> *characters)
{
    ObjectManager *objectManager = ObjectManager::get();
    {
        std::unique_lock<std::mutex> initLock(*m);
        Character *man = new Character(20, 5, 0.5, 10);
        man->setPosition(400.f, 150.f);


        objectManager->addObject(man);
        characters->push_back(man);
        inputs->push_back(std::string("")); // this will be at clients_connected - 1
    }

    zmq::context_t comtext(1);
    zmq::socket_t comsock(comtext, zmq::socket_type::rep);
    std::string endpoint("tcp://*:" + std::to_string(5555 + client_number)); // find next available local host connection
    comsock.bind(endpoint.c_str());
    std::cout << "binding to " << endpoint << std::endl;
    while (1) {
        zmq::message_t input_message;
        auto res = comsock.recv(input_message);
        {
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
        }
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window", sf::Style::Resize);
    // change the position of the window (relatively to the desktop)
    window.setPosition(sf::Vector2i(50, 50));


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
    morb.setFillColor(sf::Color(166, 126, 18));

    objectManager->addObject(&morb);

    Platform borb(sf::Vector2f(120.f, 25.f));
    borb.setPosition(375.f, 450.f);
    std::vector<sf::Vector2f> borb_set;
    borb_set.push_back(sf::Vector2f(575.f, 450.f));
    borb_set.push_back(sf::Vector2f(375.f, 450.f));
    borb.setToGo(borb_set);
    borb.setSpeed(2);
    borb.setFillColor(sf::Color(166, 126, 18));

    objectManager->addObject(&borb);

    Platform ground(sf::Vector2f(800.f, 25.f));
    ground.setPosition(0, 600 - 25);
    ground.setTexturePath("img/grass.jpg");
    

    objectManager->addObject(&ground);


    Platform wall(sf::Vector2f(25.f, 600-25));
    wall.setPosition(100, 0);

    objectManager->addObject(&wall);

    Platform wall2(sf::Vector2f(25.f, 600-25));
    wall2.setPosition(700, 0);
    
    objectManager->addObject(&wall2);

    std::vector<Character*> characters;


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
                if(!(localTime.isPaused())) {
                    characters.at(idx++)->input(input);
                }
            }
            if(!(localTime.isPaused()))
                objectManager->updateObjects();

            window.clear(sf::Color::Black);

            for (auto & [key, value] : objectManager->getVisibles()) {
                window.draw(*value);
            }
            window.display();
        }
    }
    return 0;
}
