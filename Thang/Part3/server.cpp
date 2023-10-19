#include <zmq_addon.hpp>
#include <zmq.hpp>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

int main()
{
    zmq::context_t ctx(3);
    zmq::socket_t con(ctx, zmq::socket_type::rep);
    con.bind("tcp://localhost:5555");
    int connections = 0;
    std::vector<int> started;
    int iteration = 0;

    zmq::context_t pub_ctx(5);
    zmq::socket_t pub(ctx, zmq::socket_type::pub);
    pub.bind("tcp://localhost:5550");
    using namespace std::chrono_literals;
    while (1) {
        zmq::message_t request;
        auto res = con.recv(request, zmq::recv_flags::dontwait);
        if (res) {
            connections++;
            started.push_back(iteration);
            con.send(zmq::buffer(std::to_string(connections)));
        }


        std::this_thread::sleep_for(1s);
        int process = 1;
        for (auto & start : started) {
            pub.send(zmq::buffer("Client " + std::to_string(process) + ": Iteration " + std::to_string(iteration - start + 1)));
            process++;
        }
        iteration++;
    }
    return 0;
}