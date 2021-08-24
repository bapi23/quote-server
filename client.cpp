#include <iostream>
#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <unordered_set>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <boost/circular_buffer.hpp>


#include <boost/program_options.hpp>
#include "Message.pb.h"

#include "nlohmann/json.hpp"

namespace po = boost::program_options;

struct ScenarioEntry{
    bool subscribe;
    std::string productId;
    int timeInS;
};

std::queue<std::string> endpointsToSubscribe;
std::queue<std::string> endpointsToUnsubscribe;
std::unordered_map<std::string, std::shared_ptr<std::atomic<bool>>> endpointToHandler;
std::condition_variable cvEndpoints;
std::mutex mutexEndpoints;

std::queue<std::string> feedMessagesQueue;
std::condition_variable cvFeedMessages;
std::mutex mutexFeedMessages;

std::queue<ScenarioEntry> scenarioMessagesQueue;
std::condition_variable cvScenario;
std::mutex mutexScenario;

std::string clientId;

std::queue<ScenarioEntry> scenario;

boost::circular_buffer<std::chrono::steady_clock::time_point> stamps(100);

void handleFeedMessages(){
    std::chrono::steady_clock::time_point lastTime;
    while(true){
        std::queue<std::string> newMessages;
        {
            std::unique_lock<std::mutex> lk(mutexFeedMessages);
            cvFeedMessages.wait(lk, []{return !feedMessagesQueue.empty();});
            newMessages = feedMessagesQueue;
            std::queue<std::string> emptyS;
            std::swap(feedMessagesQueue, emptyS );
        }
        while(!newMessages.empty()){
            auto message = newMessages.front();
            newMessages.pop();

            qs::qsMessage pMessage;
            pMessage.ParseFromString(message);
            if(pMessage.message_type() == qs::MessageType_ORDERBOOK){
                 std::cout << "["<< pMessage.order_book().product_id() << "] " << "Received new orderbook message" << std::endl;
            } else if(pMessage.message_type() == qs::MessageType_TRADE){
                std::cout << "["<< pMessage.trade().product_id() << "] " << " Received new trade message of type: " << pMessage.trade().type() << std::endl;
            }
        }
    }
}

void handleScenario(){
    while(!scenario.empty()){
        ScenarioEntry entry = scenario.front();
        scenario.pop();

        std::this_thread::sleep_for(std::chrono::seconds(entry.timeInS));
        {
            std::unique_lock<std::mutex> lk(mutexScenario);
            scenarioMessagesQueue.push(entry);
        }
        cvScenario.notify_one();
    }
}

std::queue<ScenarioEntry> transform(const std::vector<std::string>& vec){
    std::queue<ScenarioEntry> out;
    for(int i = 0; i < vec.size(); i+=3){
        std::cout << vec[i] << std::endl;
        std::cout << vec[i+1] << std::endl;
        std::cout << vec[i+2] << std::endl;
        bool subscribe = vec[i] == "sub"? true: false;
        std::string productId = vec[i+1];
        int timeInS = std::stoi(vec[i+2]);
        out.push(ScenarioEntry{subscribe, productId, timeInS});
    }
    return out;
}

void hendleNewEndpoints(){
    while(true){
        std::queue<std::string> currentEndpointsToSubscribe;
        std::queue<std::string> currentEndpointsToUnsubscribe;
        {
            std::unique_lock<std::mutex> lk(mutexEndpoints);
            cvEndpoints.wait(lk, []{return !endpointsToSubscribe.empty() || !endpointsToUnsubscribe.empty();});
            std::cout << "Received new endpoints" << std::endl;
            currentEndpointsToSubscribe = endpointsToSubscribe;
            std::queue<std::string> emptyS;
            std::swap(endpointsToSubscribe, emptyS );
            currentEndpointsToUnsubscribe = endpointsToUnsubscribe;
            std::queue<std::string> emptyUns;
            std::swap(endpointsToUnsubscribe, emptyUns );
        }
        while(!currentEndpointsToSubscribe.empty()){
            std::string endpoint = currentEndpointsToSubscribe.front();
            currentEndpointsToSubscribe.pop();
            std::cout << "Handling endpoint" << endpoint << std::endl;
        
            std::shared_ptr<std::atomic<bool>> at = std::make_shared<std::atomic<bool>>();
            at->store(true);
            auto t = std::thread([endpoint, at](){
                zmq::context_t ctx;
                zmq::socket_t sock(ctx, zmq::socket_type::sub);
                sock.connect(endpoint);
                sock.set( zmq::sockopt::rcvtimeo, 500 ); // milliseconds
                std::string topic = ""; //All topics
                sock.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
                std::cout << "Connected to product endpoint: " << endpoint;
                while(*at){
                    zmq::message_t message;
                    const auto ret = sock.recv(message, zmq::recv_flags::none);

                    if(message.size() == 0 || message.data() == nullptr){
                        std::cout << "Received empty message" << std::endl;
                        continue;
                    }
                    auto data = std::string(static_cast<char*>(message.data()), message.size());
                    {
                        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                        stamps.push_back(begin);

                        if(stamps.size() > 0){
                            auto last = stamps.back();
                            auto first = stamps.front();
                            auto difference_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last - first).count();
                            auto messages_per_second = boost::numeric_cast<float>(difference_ms)/boost::numeric_cast<float>(stamps.size()) * 1000;
                            std::cout << "Feed frequency = " << messages_per_second << " messages/s" << std::endl;
                            // Send metrics
                        }

                        std::unique_lock<std::mutex> lk(mutexFeedMessages);
                        feedMessagesQueue.push(data);
                    }
                    cvFeedMessages.notify_one();
                }
            });

            t.detach();
            endpointToHandler.emplace(endpoint, at);
        }
        while(!currentEndpointsToUnsubscribe.empty()){
            std::string endpoint = currentEndpointsToUnsubscribe.front();
            currentEndpointsToUnsubscribe.pop();
            (*endpointToHandler[endpoint]).store(false);
        }
    }
}

int main(int argc, char **argv){
    po::variables_map vm;
    std::vector<std::string> prodIds;
    std::string serverEndpoint;

    po::options_description desc("Client");
    desc.add_options()("help", "help msg");
    desc.add_options()("prod-ids", po::value<std::vector<std::string> >()->multitoken(), "symbol of the product ids to subscribe or unsubscribe with time[s] in format 'sub ETH-USD 1'");
    desc.add_options()("server-endpoint", po::value<std::string>(),
                        "Server endpoint in format tcp://<ip-address>:<port>");
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("prod-ids")) {
        prodIds = vm["prod-ids"].as<std::vector<std::string>>();
    } else {
        std::cout << "Please provide prod-id" << std::endl;
        return 1;
    }

    if (vm.count("server-endpoint")) {
        serverEndpoint = vm["server-endpoint"].as<std::string>();
    } else {
        std::cout << "Please provide server-endpoint" << std::endl;
        return 1;
    }

    scenario = transform(prodIds);

    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.set(zmq::sockopt::rcvtimeo, 500); //in milliseconds
    sock.connect(serverEndpoint);

    nlohmann::json jmessage = {
        {"type", "client_id_request"}
    };
    std::string data = jmessage.dump();
    zmq::message_t message(data.size());
    memcpy (message.data(), data.data(), data.size());
    if(!sock.send(message, zmq::send_flags::none)){
        std::cout << "Can't send initital (client_id_request) message!"<< std::endl;
        return 1;
    }
    std::cout << "Sent client_id_request message: " <<data<< std::endl;

    std::thread(&hendleNewEndpoints).detach();
    std::thread(&handleFeedMessages).detach();
    std::thread(&handleScenario).detach();

    while(true){
        using namespace std::chrono_literals;
        zmq::message_t message;
        //std::cout << "Waiting for the message" << std::endl;
        const auto ret = sock.recv(message, zmq::recv_flags::none);

        if(message.size() == 0 || message.data() == nullptr){
            std::cout << "Haven't received message for 500ms - service is down! Will try to retrigger in 1s" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        auto data = std::string(static_cast<char*>(message.data()), message.size());

        const auto jmsg = nlohmann::json::parse(data);
        if(jmsg.contains("type")){
            if(jmsg["type"] == "client_id_request"){
                std::cout << "Received client_id_request response message!"<< std::endl;
                clientId = jmsg["client_id"];
            } else if(jmsg["type"] == "subscribe_request"){
                {
                    std::unique_lock<std::mutex> lk(mutexEndpoints);
                    std::cout << "Received subscribe confirmation message!"<< std::endl;
                    endpointsToSubscribe.push(jmsg["product_endpoint"]);
                    std::cout << endpointsToSubscribe.size();
                }
                cvEndpoints.notify_one();
            } else if(jmsg["type"] == "unsubscribe_request"){
                std::cout << "Received unsubscribe confirmation message!"<< std::endl;
                {
                    std::unique_lock<std::mutex> lk(mutexEndpoints);
                    endpointsToUnsubscribe.push(jmsg["product_endpoint"]);
                }
                cvEndpoints.notify_one();
            } else if (jmsg["type"] == "heartbeat"){
                //std::cout << "Received heartbeat message!"<< std::endl;
            } else {
                std::cout << "Received unuspported message type: "<< jmsg["type"]<< std::endl;
            }
        } else {
            std::cout << "Received unsupported message!"<< std::endl;
        }

        std::string response;
        {
            std::unique_lock<std::mutex> lk(mutexScenario);
            cvScenario.wait_for(lk, 10ms, []{return !scenarioMessagesQueue.empty();});
            if(!scenarioMessagesQueue.empty())
            {
                auto scenarioEntry = scenarioMessagesQueue.front();
                scenarioMessagesQueue.pop();

                std::cout << "sending scenario message: " << response;
                
                nlohmann::json jmessage;
                if(scenarioEntry.subscribe){
                    jmessage =
                    {{"type", "subscribe_request"},
                    {"client_id", clientId},
                    {"product_id", scenarioEntry.productId}
                    };
                } else {
                    jmessage =
                    {{"type", "unsubscribe_request"},
                    {"client_id", clientId},
                    {"product_id", scenarioEntry.productId}
                    };
                }

                response = jmessage.dump();
            }
        }
        if(response.empty()){ // no scenario message
            nlohmann::json jmessage = {
                {"type", "heartbeat"},
                {"client_id", clientId}
            };
            response = jmessage.dump();               
        }

        zmq::message_t outMessage(response.size());
        memcpy (outMessage.data(), response.data(), response.size());
        if(!sock.send(outMessage, zmq::send_flags::none)){
            std::cout << "Can't send response message!"<< std::endl;
            return 1;
        }
    }
}