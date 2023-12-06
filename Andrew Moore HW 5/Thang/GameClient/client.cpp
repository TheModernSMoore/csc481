#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include "objects/manager/objectManager.h"
#include "timeline/timeManager.h"
#include <chrono>
#include <thread>

using json = nlohmann::json;

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

std::string getInput()
{
    std::string output;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        output += std::string("l");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        output += std::string("r");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        output += std::string("u");
    }
    return output;
}

int main(int argc, char const *argv[])
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window", sf::Style::Resize);
    sf::View view(sf::Vector2f(400, 300), sf::Vector2f(800, 600));
    // change the position of the window (relatively to the desktop)
    window.setPosition(sf::Vector2i(50, 50));

    // TimeManager *timeManager = TimeManager::get();
    // Timeline realTime = Timeline();
    // Timeline localTime = Timeline(&realTime, 100000000);
    // timeManager->addTimeline(&realTime);
    // timeManager->addTimeline(&localTime);

    ObjectManager *objectManager = ObjectManager::get();
    

    zmq::context_t connecxt(1);
    zmq::socket_t connect_checker(connecxt, zmq::socket_type::req);
    connect_checker.connect("tcp://localhost:5555");
    connect_checker.send(zmq::buffer("Ready to connect.")); // Message not used, just used to see when someone connects

    zmq::message_t reply;
    auto res = connect_checker.recv(reply);
    int client_number = -1;

    if(sscanf(reply.to_string().c_str(), "%d", &client_number) <= 0) {
        std::cout << reply.to_string() << std::endl;
        std::cout << "cringe " << client_number << std::endl;
    };
    
    zmq::context_t maintext(1);
    zmq::socket_t mainket(maintext, zmq::socket_type::req);
    std::string endpoint("tcp://localhost:" + std::to_string(5555 + client_number));
    mainket.connect(endpoint.c_str());
    std::cout << "connecting to " << endpoint << std::endl;

    mainket.send(zmq::buffer("gimmie my character id please"));

    zmq::message_t char_ident_msg;
    res = mainket.recv(char_ident_msg);
    int character_ident = -1;

    if(sscanf(char_ident_msg.to_string().c_str(), "%d", &character_ident) <= 0) {
        std::cout << reply.to_string() << std::endl;
        std::cout << "cringe " << character_ident << std::endl;
    };

    
    int input_timer = 3;
    bool p_press = false;
    bool esc_press = false;
    bool in_focus = true;
    Character *my_character = nullptr;


    // for (int i = 0; i < 5; i++)  
    // {
    //     realTime.setTime();
    //     localTime.setTime();
    // }

    // run the program as long as the window is open
    while (window.isOpen())
    {
        std::string input;
        

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                    p_press = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    esc_press = true;
                }
            }
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::LostFocus)
                in_focus = false;
            else if (event.type == sf::Event::GainedFocus)
                in_focus = true;
        }

        if (p_press || esc_press) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (input_timer <= 0) {
                if (p_press && esc_press) {
                    json color_json;
                    color_json["Type"] = COLOR;
                    mainket.send(zmq::buffer(to_string(color_json)));
                    zmq::message_t idc;
                    auto res = mainket.recv(idc);
                    input_timer = 3;
                    p_press = false;
                    esc_press = false;
                } else if (p_press) {
                    // Create Cycle Speed event
                    json speed_json;
                    speed_json["Type"] = CYCLE_SPEED;
                    mainket.send(zmq::buffer(to_string(speed_json)));
                    zmq::message_t idc;
                    auto res = mainket.recv(idc);
                    input_timer = 3;
                    p_press = false;
                } else if (esc_press) {
                    // Create Pause Time event
                    json pause_json;
                    pause_json["Type"] = PAUSE;
                    mainket.send(zmq::buffer(to_string(pause_json)));
                    zmq::message_t idc;
                    auto res = mainket.recv(idc);
                    input_timer = 3;
                    esc_press = false;
                }
            } else {
                // I know this is uber sus but I think my timeline is uber sus
                input_timer -= 1;
            }
        }

        if(in_focus) {
            input += getInput();
        }

        // MAYBE DO SIMILAR THING THAT SERVER DOES WHERE WE HAVE A SEPERATE THREAD THAT HANDLES INTERACTIONS THROUGH THE SOCKETS THEN HAVE CLIENT CALCULATIONS DONE IN MAIN

        // SEND OVER EVENTS HERE WITH SEND MORE AS WELL
        json inp_json;
        inp_json["Type"] = -1;
        inp_json["Input"] = input;
        mainket.send(zmq::buffer(to_string(inp_json)));

        // clear the window with black color
        window.clear(sf::Color::Black);
        
        std::vector<int> identifiers_recv;
        std::vector<Object *> objects_not_recv;

        // Get json, and check if they have more messages to keep reading more input
        bool more = true;
        while (more) {
            zmq::message_t obj_msg;
            auto res = mainket.recv(obj_msg);
            json to_parse = json::parse(obj_msg.to_string());
            identifiers_recv.push_back(objectManager->parseObjJSON(to_parse));
            more = obj_msg.more();
        }
        // DELETE OBJECTS THAT WERE NOT SENT!!!!!
        std::vector<int>::iterator it;
        it = identifiers_recv.begin();
        for (auto & [ident, object] : objectManager->getObjects()) {
            if (ident == *it) {
                it++;
                if (object->visible) {
                window.draw(*object);
            }
            } else {
                objects_not_recv.push_back(object);
            }
            if (ident == character_ident) {
                my_character = dynamic_cast<Character *>(object);
            }
        }
        for (auto & object : objects_not_recv) {
            objectManager->removeObject(object);
        }

        // HANDLE VIEW STUFF HERE
        view.move((my_character->getPosition().x - view.getCenter().x) / 50, 0);
        window.setView(view);   
    
        window.display();
    }
    return 0;
}