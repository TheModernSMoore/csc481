#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include "objects/manager/objectManager.h"
#include "timeline/timeManager.h"

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
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
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

    bool in_focus = true;
    bool paused = false;

    Character *my_character = nullptr;

    // // do not ask why this is necessary, IF I WERE TO CREATE 1 MORE OBJECT, THEY WOULD NOT APPEAR
    // // AND THIS FIXED IT
    // for (int i = 0; i < W; i++)  
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
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                    input += std::string("c"); // This will be deleted once parsed
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    input += std::string("p"); // This will be deleted once parsed
                }
            }
            if (event.type == sf::Event::LostFocus)
                in_focus = false;
            else if (event.type == sf::Event::GainedFocus)
                in_focus = true;
                
        }

        if(in_focus) {
            input += getInput();
        }

        // MAYBE DO SIMILAR THING THAT SERVER DOES WHERE WE HAVE A SEPERATE THREAD THAT HANDLES INTERACTIONS THROUGH THE SOCKETS THEN HAVE CLIENT CALCULATIONS DONE IN MAIN
        mainket.send(zmq::buffer(input));

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
            if (!my_character && ident == character_ident) {
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