#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>

#include "utils/uuid.hpp"

class ClientRegister{
    void run(){
        std::thread(std::bind(ClientRegister::handleRegisterMessages, this)).detach();
    }

    void handleRegisterMessages(){
        zmq::context_t ctx;
        zmq::socket_t sock(ctx, zmq::socket_type::rep);
        sock.bind("tcp://*:9090");
        while(isRunning){         
            zmq::message_t message;
            const auto ret = sock.recv(message, zmq::recv_flags::dontwait);

            if(message.size() == 0 || message.data() == nullptr){
                std::cout << "Received empty message";
                continue;
            }
                            }
            auto data = std::string(static_cast<char*>(message.data()), message.size());
  
            auto jmsg = nlohmann::json::parse(data);
            if(jmsg.contains("type")){
                if(jmsg.contains("cliend_id_request")){
                    std::string data = "{'client_id': '" + uuid::generate_uuid() + "', 'status': 'registered'}";
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else if (jmsg.contains("subscribe_request")){
                    std::string product_id = jmsg["product_id"].get<std::string>();
                    std::string data = "{'client_id': " + uuid::generate_uuid() + "', 'subscribed': " + product_id + " }";
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else if (jmsg.contains("unsubscribe_request")){
                    std::string product_id = jmsg["product_id"].get<std::string>();
                    std::string data = "{'client_id': " + uuid::generate_uuid() + "', 'unsubscribed': " + product_id + " }";
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                }
            }
        }
    }

    std::atomic<bool> isRunning{true};
    std::vector<std::string> registeredClientIds;
    std::vector<std::string> registerInProgresIds;
}