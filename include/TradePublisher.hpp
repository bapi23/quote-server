#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <boost/numeric/conversion/cast.hpp>

#include "ZMQPublisher.hpp"
#include "product/ProductIdConnectionTable.hpp"
#include "product/ProductChangePublisher.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Message.pb.h"

using json = nlohmann::json;
#define PROTOCOL_BUF_PUB

class TradePublisher
{
public:
    TradePublisher(const std::string& productId):
        m_publisher(transport::EndpointProvider::getInstance().prodIdToPort(productId).address + ":" + transport::EndpointProvider::getInstance().prodIdToPort(productId).tradePort)
        {
        }

    void publish(std::unique_ptr<Trade> trade){
        std::string payload = trade->generateMessage();

        m_publisher.publish(payload);
    }

    ZMQPublisher m_publisher;
};