
#include <zmq_addon.hpp>
#include <iostream>

int main(){
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::sub);
    sock.connect("tcp://127.0.0.1:11111");
    std::string topic = "";
    sock.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    while(true){
        zmq::message_t message;
        std::cout << "Waiting for feed message"<< std::endl;
        const auto ret = sock.recv(message, zmq::recv_flags::none);
        auto data = std::string(static_cast<char*>(message.data()), message.size());
        std::cout << data << std::endl;
    }
}