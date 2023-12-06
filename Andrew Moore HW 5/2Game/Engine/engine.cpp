#include <SFML/Window.hpp>
#include "objects/manager/objectManager.h"
#include "timeline/timeManager.h"
#include "events/manager/eventManager.h"
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
    Character *man = new Character(20, 5, 0.5, 10);
    man->setPosition(400, 500);
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
    // sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window", sf::Style::Resize);
    // // change the position of the window (relatively to the desktop)
    // window.setPosition(sf::Vector2i(50, 50));


    TimeManager *timeManager = TimeManager::get();
    Timeline realTime = Timeline();
    Timeline localTime = Timeline(&realTime, 65000);
    timeManager->addTimeline(&realTime);
    timeManager->addTimeline(&localTime);


    ObjectManager *objectManager = ObjectManager::get();


    Platform wall(sf::Vector2f(25.f, 600-25));
    wall.setPosition(-25, 0);
    wall.setFillColor(sf::Color::Red);

    objectManager->addObject(&wall);

    Platform wall2(sf::Vector2f(25.f, 600-25));
    wall2.setPosition(800, 0);
    wall2.setFillColor(sf::Color::Red);

    objectManager->addObject(&wall2);

    SpawnPoint spawner2;
    spawner2.setPosition(400, 700);

    objectManager->addObject(&spawner2);

    Enemy enemy0(sf::Vector2f(40, 40));
    enemy0.setPosition(200, 100);

    objectManager->addObject(&enemy0);

    Enemy enemy1(sf::Vector2f(40, 40));
    enemy1.setPosition(300, 100);
    
    objectManager->addObject(&enemy1);

    Enemy enemy2(sf::Vector2f(40, 40));
    enemy2.setPosition(400, 100);

    objectManager->addObject(&enemy2);

    Enemy enemy3(sf::Vector2f(40, 40));
    enemy3.setPosition(500, 100);

    objectManager->addObject(&enemy3);

    Enemy enemy4(sf::Vector2f(40, 40));
    enemy4.setPosition(600, 100);

    objectManager->addObject(&enemy4);

    Enemy enemy5(sf::Vector2f(40, 40));
    enemy5.setPosition(200, 0);

    objectManager->addObject(&enemy5);

    Enemy enemy6(sf::Vector2f(40, 40));
    enemy6.setPosition(300, 0);
    
    objectManager->addObject(&enemy6);

    Enemy enemy7(sf::Vector2f(40, 40));
    enemy7.setPosition(400, 0);

    objectManager->addObject(&enemy7);

    Enemy enemy8(sf::Vector2f(40, 40));
    enemy8.setPosition(500, 0);

    objectManager->addObject(&enemy8);

    Enemy enemy9(sf::Vector2f(40, 40));
    enemy9.setPosition(600, 0);

    objectManager->addObject(&enemy9);


    std::vector<Character*> characters;


    std::mutex m;


    std::vector<std::string> inputs;
    int clients_connected = 0;

    std::thread client_creator(clientCreation, &clients_connected, &inputs, &m, &characters);


    EventManager *eventManager = EventManager::get();

    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_COLLISION}, new CharCollideHandler);
    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_DEATH}, new CharDeathHandler);
    eventManager->addEventToHandler(std::list<EventType> {CHARACTER_SPAWN}, new CharSpawnHandler);
    eventManager->addEventToHandler(std::list<EventType> {USER_INPUT}, new InputHandler);
    eventManager->addEventToHandler(std::list<EventType> {PAUSE}, new PauseHandler);
    eventManager->addEventToHandler(std::list<EventType> {CYCLE_SPEED}, new SpeedHandler);
    eventManager->addEventToHandler(std::list<EventType> {ENEMY_COLLISION}, new EnCollideHandler);
    eventManager->addEventToHandler(std::list<EventType> {BULLET_SHOOT}, new BulletHandler);
    eventManager->addEventToHandler(std::list<EventType> {BULLET_DEATH}, new BulletDeathHandler);



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
    return 0;
}
