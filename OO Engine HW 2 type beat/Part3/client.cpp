#include <iostream>
#include <zmq_addon.hpp>
#include <zmq.hpp>


int main(int argc, char const *argv[])
{
    zmq::context_t ctx(3);
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.connect("tcp://localhost:5555");
    sock.send(zmq::buffer("ready"));
    zmq::message_t output;
    auto res = sock.recv(output);
    
    zmq::context_t subtex(5);
    zmq::socket_t sub(subtex, zmq::socket_type::sub);
    sub.connect("tcp://localhost:5550");
    std::string identifier = output.to_string();
    sub.set(zmq::sockopt::subscribe, std::string("Client " + identifier));

    while (1) {
        zmq::message_t outers;
        auto res = sub.recv(outers, zmq::recv_flags::none);

        std::string output = outers.to_string();
        int erase;
        sscanf(outers.to_string().c_str(), "%*d %n", &erase);
        output.erase(0, erase);

        std::cout << output << std::endl;
    }
    return 0;
}