#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <atomic>

#include "utils/uuid.hpp"
#include "ProductIdConnectionTable.hpp"
#include "ProductSubscriber.hpp"

class ClientRegister{
public:
    ClientRegister(ProductSubscriber* productSubscriber, const std::string& endpoint_address):
    m_productSubscriber(productSubscriber),
    m_endpoint_address(endpoint_address)
    {
    }

    void run(){
        std::thread(std::bind(&ClientRegister::runImpl, this)).detach();
    }

private:
    void runImpl(){
        zmq::context_t ctx;
        zmq::socket_t sock(ctx, zmq::socket_type::rep);
        sock.bind("tcp://*:9090");
        while(isRunning){         
            zmq::message_t message;
            const auto ret = sock.recv(message, zmq::recv_flags::none);

            if(message.size() == 0 || message.data() == nullptr){
                std::cout << "Received empty message" << std::endl;
                continue;
            }
            auto data = std::string(static_cast<char*>(message.data()), message.size());
  
            auto jmsg = nlohmann::json::parse(data);
            if(jmsg.contains("type")){
                if(jmsg["type"] == "cliend_id_request"){
                    const std::string clientId = uuid::generate_uuid();
    
                    nlohmann::json jmessage =
                        {"cline_id_request", {
                                {"client_id", clientId},
                                {"status", "registered"},
                            }
                        };
                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                    registeredClientIds.insert(clientId);

                } else if (jmsg["type"] =="subscribe_request"){
                    if(!jmsg.contains("client_id")){
                        std::cout << "Invalid subscribe_request = got no client_id" << std::endl;
                        continue;
                    }

                    if(!jmsg.contains("product_id")){
                        std::cout << "Invalid subscribe_request = got no product_id" << std::endl;
                        continue;
                    }

                    const std::string clientId = jmsg["client_id"];
                    const std::string productId = jmsg["product_id"];
                    if(registeredClientIds.count(clientId) == 0){
                        std::cout << "Invalid subscribe_request server don't know the client id: " << clientId << std::endl;
                        continue;
                    }

                    const bool canSubscribe = transport::prodIdToPort.count(productId) > 0;
    
                    nlohmann::json jmessage =
                        {"subscribe_request", {
                                {"client_id", clientId},
                                {"status", canSubscribe? "subscribed": "error"},
                                {"product_id", productId},
                                {"product_endpoint", "tcp://" + m_endpoint_address + transport::prodIdToPort[productId]}
                            }
                        };

                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else if (jmsg["type"] =="unsubscribe_request"){
                    if(!jmsg.contains("client_id")){
                        std::cout << "Invalid subscribe_request = got no client_id" << std::endl;
                        continue;
                    }

                    if(!jmsg.contains("product_id")){
                        std::cout << "Invalid subscribe_request = got no product_id" << std::endl;
                        continue;
                    }

                    const std::string clientId = jmsg["client_id"];
                    const std::string productId = jmsg["product_id"];
                    if(registeredClientIds.count(clientId) == 0){
                        std::cout << "Invalid subscribe_request server don't know the client id: " << clientId << std::endl;
                        continue;
                    }

                    const bool canUnsubscribe = transport::prodIdToPort.count(productId) > 0;

                    nlohmann::json jmessage =
                        {"unsubscribe_request", {
                                {"client_id", clientId},
                                {"status", canUnsubscribe? "subscribed": "error"},
                                {"product_id", productId},
                            }
                        };

                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                }
            }
        }
    }

std::atomic<bool> isRunning{true};
std::unordered_set<std::string> registeredClientIds;
ProductSubscriber* m_productSubscriber;
std::string m_endpoint_address;
};