#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include "objects/platform.h"
#include "objects/character.h"
#include "timeline/timeManager.h"

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
    // change the position of the window (relatively to the desktop)
    window.setPosition(sf::Vector2i(50, 50));

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

    bool in_focus = true;
    bool paused = false;

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

        mainket.send(zmq::buffer(input));

        // clear the window with black color
        window.clear(sf::Color::Black);
        


        for (auto & object : objectManager->getObjects()) {
            zmq::message_t obj_msg;
            auto res = mainket.recv(obj_msg);
            object->parseString(obj_msg.to_string());
            window.draw(*object);
        }
        
        window.display();
    }
    return 0;
}