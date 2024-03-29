#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <atomic>

#include "ClientService.hpp"
#include "utils/uuid.hpp"
#include "CoinbaseEndpointService.hpp"
#include "product/ProductSubscriber.hpp"

ClientService::ClientService(ProductSubscriber* productSubscriber, const std::string& endpoint_address):
    m_productSubscriber(productSubscriber),
    m_endpoint_address(endpoint_address)
{
}

void ClientService::run(){
    std::thread(std::bind(&ClientService::runImpl, this)).detach();
}

void ClientService::runImpl(){
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.bind("tcp://" + m_endpoint_address);
    while(isRunning){         
        zmq::message_t message;
        //std::cout << "Waiting for the message" << std::endl;
        const auto ret = sock.recv(message, zmq::recv_flags::none);

        auto data = std::string(static_cast<char*>(message.data()), message.size());
        try{
            auto jmsg = nlohmann::json::parse(data);
            if(jmsg.contains("type")){
                if(jmsg["type"] == "client_id_request"){
                    std::cout << "Received client id request" << std::endl;
                    const std::string clientId = uuid::generate_uuid();
    
                    nlohmann::json jmessage ={
                        {"type", "client_id_request"},
                        {"client_id", clientId},
                        {"status", "registered"}
                    };
                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                    std::cout << "Sent client id request response" << std::endl;
                    m_registeredClientIds.insert(clientId);
                    std::cout << "Registered already " << m_registeredClientIds.size() << " clients" << std::endl;

                } else if (jmsg["type"] =="subscribe_request"){
                    std::cout << "Received subscribe_request" << std::endl;
                    std::string error;
                    if(!jmsg.contains("client_id")){
                        std::cout << "Invalid subscribe_request = got no client_id" << std::endl;
                        error = "MalformedMsgNoClient";
                        continue;
                    }

                    if(!jmsg.contains("product_id")){
                        std::cout << "Invalid subscribe_request = got no product_id" << std::endl;
                        error = "MalformedMsgNoProduct";
                        continue;
                    }

                    const std::string clientId = jmsg["client_id"];
                    const std::string productId = jmsg["product_id"];
                    if(m_registeredClientIds.count(clientId) == 0){
                        std::cout << "Invalid subscribe_request server don't know the client id: " << clientId << std::endl;
                        error = "UnknownClientId";
                    }

                    if(!transport::EndpointProvider::getInstance().contains(productId)){
                        std::cout << "Invalid subscribe_request server don't know the product_id: " << clientId << std::endl;
                        error = "UnknownProductId";
                    }

                    std::string data;
                    if(!error.empty()){
                        nlohmann::json jmessage = {
                            {"type", "error"},
                            {"error", error},
                            // TODO send full list of subscription
                        };
                        data = jmessage.dump();
                    } else {
                        m_productSubscriber->subscribe(clientId, productId);
                        m_registeredClientIds.insert(clientId);

                        nlohmann::json jmessage = {
                                {"type", "subscribe_request"},
                                {"client_id", clientId},
                                {"product_id", productId},
                                {"product_endpoint", "tcp://" + transport::EndpointProvider::getInstance().prodIdToPort(productId).address + ":" + transport::EndpointProvider::getInstance().prodIdToPort(productId).orderbookPort},
                                {"trade_endpoint", "tcp://" + transport::EndpointProvider::getInstance().prodIdToPort(productId).address + ":" + transport::EndpointProvider::getInstance().prodIdToPort(productId).tradePort}
                                // TODO send full list of subscription
                            };
                        data = jmessage.dump();
                    }

                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else if (jmsg["type"] =="unsubscribe_request"){
                    std::cout << "Received unsubscribe_request" << std::endl;
                    std::string error;
                    if(!jmsg.contains("client_id")){
                        std::cout << "Invalid unsubscribe_request = got no client_id" << std::endl;
                        error = "MalformedMsgNoClient";
                    }

                    if(!jmsg.contains("product_id")){
                        std::cout << "Invalid unsubscribe_request = got no product_id" << std::endl;
                        error = "MalformedMsgNoProduct";
                    }

                    const std::string clientId = jmsg["client_id"];
                    const std::string productId = jmsg["product_id"];
                    if(m_registeredClientIds.count(clientId) == 0){
                        std::cout << "Invalid unsubscribe_request server don't know the client id: " << clientId << std::endl;
                        error = "UnknownClientId";
                    }

                    if(! transport::EndpointProvider::getInstance().contains(productId)){
                        std::cout << "Invalid unsubscribe_request server don't know the product_id: " << clientId << std::endl;
                        error = "UnknownProductId";
                    }

                    std::string data;
                    if(!error.empty()){
                        nlohmann::json jmessage = {
                            {"type", "error"},
                            {"error", error},
                            // TODO send full list of subscription
                        };
                        data = jmessage.dump();
                        std::cout << "Got error while reading unsibscribe message" << error << std::endl;
                    } else {
                        std::cout << "UBSUBSCIRBING" << std::endl;
                        m_productSubscriber->unsubscribe(clientId, productId);

                        nlohmann::json jmessage = {
                                {"type", "unsubscribe_request"},
                                {"client_id", clientId},
                                {"product_id", productId},
                                {"product_endpoint", "tcp://" + transport::EndpointProvider::getInstance().prodIdToPort(productId).address + ":" + transport::EndpointProvider::getInstance().prodIdToPort(productId).orderbookPort},
                                {"trade_endpoint", "tcp://" + transport::EndpointProvider::getInstance().prodIdToPort(productId).address + ":" + transport::EndpointProvider::getInstance().prodIdToPort(productId).tradePort}
                                // TODO send full list of subscription
                            };
                        data = jmessage.dump();
                    }

                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else if (jmsg["type"] =="heartbeat"){
                    
                    std::string error;
                    if(!jmsg.contains("client_id")){
                        std::cout << "Invalid subscribe_request = got no client_id" << std::endl;
                        error = "MalformedMsgNoClient";
                    }

                    const std::string clientId = jmsg["client_id"];
                    if(m_registeredClientIds.count(clientId) == 0){
                        std::cout << "Invalid subscribe_request server don't know the client id: " << clientId << std::endl;
                        error = "UnknownClientId";
                    }

                    std::string data;
                    if(!error.empty()){
                        nlohmann::json jmessage = {
                            {"type", "error"},
                            {"error", error},
                            // TODO send full list of subscription
                        };
                        data = jmessage.dump();
                    } else {
                        nlohmann::json jmessage ={
                            {"type", "heartbeat"}, 
                            {"client_id", clientId}
                        };
                        data = jmessage.dump();
                    }

                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                } else {
                    std::cout << "Received unsupported message type sending back error" << std::endl;

                    nlohmann::json jmessage = {
                        {"type", "error"},
                        {"error", "usnsupported message type"},
                        {"message_type", jmsg["type"]}
                    };

                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
                }
            } else {
                    std::cout << "Received unsupported message sending back error" << std::endl;

                    nlohmann::json jmessage =
                        {
                            {"type", "error"},
                            {"error", "usnsupported message"}
                        };

                    std::string data = jmessage.dump();
                    zmq::message_t message(data.size());
                    memcpy (message.data(), data.data(), data.size());
                    sock.send(message, zmq::send_flags::none);
            }
        }
        catch(nlohmann::detail::parse_error){
            std::cout << "Received invalid message - parse error sending back error" << std::endl;

            nlohmann::json jmessage ={
                {"type", "error"}, 
                {"error", "Invalid message"}
            };

            std::string data = jmessage.dump();
            zmq::message_t message(data.size());
            memcpy (message.data(), data.data(), data.size());
            sock.send(message, zmq::send_flags::none);
        }
    }
}