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


#include <boost/program_options.hpp>

#include "nlohmann/json.hpp"

namespace po = boost::program_options;



std::queue<std::string> endpointsToSubscribe;
std::queue<std::string> endpointsToUnsubscribe;
std::unordered_map<std::string, std::shared_ptr<std::atomic<bool>>> endpointToHandler;
std::condition_variable cvEndpoints;
std::mutex mutexEndpoints;

std::queue<std::string> feedMessagesQueue;
std::condition_variable cvFeedMessages;
std::mutex mutexFeedMessages;

std::queue<std::string> scenarioMessagesQueue;
std::condition_variable cvScenario;
std::mutex mutexScenario;

std::string clientId;

void handleScenario(){
    
}

void handleFeedMessages(){
    while(true){
        std::queue<std::string> newMessages;
        {
            std::unique_lock<std::mutex> lk(mutexFeedMessages);
            cvFeedMessages.wait(lk, []{return !feedMessagesQueue.empty();});
            newMessages = feedMessagesQueue;
        }
        while(!newMessages.empty()){
            auto message = newMessages.front();
            newMessages.pop();
            std::cout << "Received new feed message" << message;
        }
    }
}

void hendleNewEndpoints(){
    while(true){
        std::unique_lock<std::mutex> lk(mutexEndpoints);
        cvEndpoints.wait(lk, []{return !endpointsToSubscribe.empty() || !endpointsToUnsubscribe.empty();});
        while(!endpointsToSubscribe.empty()){
            std::string endpoint = endpointsToSubscribe.front();
            endpointsToSubscribe.pop();
        
            std::shared_ptr<std::atomic<bool>> at = std::make_shared<std::atomic<bool>>();
            auto t = std::thread([endpoint, at](){
                zmq::context_t ctx;
                zmq::socket_t sock(ctx, zmq::socket_type::sub);
                sock.connect(endpoint);
                sock.set( zmq::sockopt::rcvtimeo, 500 ); // milliseconds
                while(*at){
                    zmq::message_t message;
                    const auto ret = sock.recv(message, zmq::recv_flags::none);

                    if(message.size() == 0 || message.data() == nullptr){
                        std::cout << "Received empty message" << std::endl;
                        continue;
                    }
                    auto data = std::string(static_cast<char*>(message.data()), message.size());
                    {
                        std::unique_lock<std::mutex> lk(mutexFeedMessages);
                        feedMessagesQueue.push(data);
                    }
                    cvFeedMessages.notify_one();
                }
            });

            t.detach();
            endpointToHandler.emplace(endpoint, at);
        }
        while(!endpointsToUnsubscribe.empty()){
            std::string endpoint = endpointsToUnsubscribe.front();
            endpointsToUnsubscribe.pop();
            (*endpointToHandler[endpoint]).store(false);
        }
    }
}

struct ScenarioEntry{
    bool subscribe;
    std::string productId;
    int timeInS;
};

std::queue<ScenarioEntry> transform(const std::vector<std::string>& vec){
    std::queue<ScenarioEntry> out;
    for(int i = 0; i < vec.size(); i+=3){
        bool subscribe = vec[i] == "sub"? true: false;
        std::string productId = vec[i+1];
        int timeInS = std::stoi(vec[i+2]);
        out.push(ScenarioEntry{subscribe, productId, timeInS});
    }
}

int main(int argc, char **argv){
    po::variables_map vm;
    std::vector<std::string> prodIds;
    std::string serverEndpoint;

    po::options_description desc("Client");
    desc.add_options()("help", "help msg");
    desc.add_options()("prod-ids", po::value<std::vector<int> >()->multitoken(), "symbol of the product ids to subscribe or unsubscribe with time[s] in format 'sub ETH-USD 1'");
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

    std::queue<ScenarioEntry> scenario = transform(prodIds);

    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::req);
    sock.connect(serverEndpoint);

    nlohmann::json jmessage = {
        {"type", "cliend_id_request"}
    };
    std::string data = jmessage.dump();
    zmq::message_t message(data.size());
    memcpy (message.data(), data.data(), data.size());
    if(!sock.send(message, zmq::send_flags::none)){
        std::cout << "Can't send initital (client_id_request) message!";
        return 1;
    }

    std::thread(&hendleNewEndpoints).detach();
    std::thread(&handleFeedMessages).detach();

    while(true){
        using namespace std::chrono_literals;
        zmq::message_t message;
        const auto ret = sock.recv(message, zmq::recv_flags::none);

        if(message.size() == 0 || message.data() == nullptr){
            std::cout << "Received empty message" << std::endl;
            continue;
        }
        auto data = std::string(static_cast<char*>(message.data()), message.size());

        const auto jmsg = nlohmann::json::parse(data);
        if(jmsg.contains("type")){
            if(jmsg["type"] == "client_id_request"){
                clientId = jmsg["client_id"];
            } else if(jmsg["type"] == "subscribe_request"){
                std::unique_lock<std::mutex> lk(mutexEndpoints);
                endpointsToSubscribe.push(jmsg["endpoint_address"]);
                cvEndpoints.notify_one();
            } else if(jmsg["type"] == "unsubscribe_request"){
                endpointsToUnsubscribe.push(jmsg["endpoint_address"]);
            }
        } else {
            std::cout << "Received unsupported message!";
        }

        if(!scenario.empty()){
            auto step = scenario.front();
            scenario.pop();
            std::this_thread::sleep_for(std::chrono::seconds(step.timeInS));

        }

    }
}