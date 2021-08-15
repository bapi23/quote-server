
#include <zmq_addon.hpp>

int main(){
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::pub);
    sock.bind("tcp://127.0.0.1:11111");
    std::string topic = "";
    while(true){
        std::string payload = "test";
        zmq::message_t message(payload.size());
        memcpy (message.data(), payload.data(), payload.size());
        sock.send(message, zmq::send_flags::none);
    }
}