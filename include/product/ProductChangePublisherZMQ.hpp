#pragma once

#include <iostream>
#include <thread>
#include <zmq_addon.hpp>
#include <nlohmann/json.hpp>
#include <atomic>
#include <boost/numeric/conversion/cast.hpp>

#include "ZMQPublisher.hpp"
#include "ProductIdConnectionTable.hpp"
#include "product/ProductChangePublisher.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using json = nlohmann::json;


class ProductChangePublisherZMQ: public ProductChangePublisher
{
public:
    ProductChangePublisherZMQ(const std::string& productId):
        m_productId(productId),
        m_stamps(2000),
        m_publisher(productId)
        {
        }

    void publish(OrderBookView* view){
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        m_stamps.push_back(begin);
        if(m_stamps.size() > 0){
            auto last = m_stamps.back();
            auto first = m_stamps.front();
            auto difference_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last - first).count();
            auto messages_per_second = boost::numeric_cast<float>(difference_ms)/boost::numeric_cast<float>(m_stamps.size()) * 1000;
            std::cout << "[" << m_productId << "] " << "Feed frequency = " << messages_per_second << " messages/s" << std::endl;
            // Send metrics
        }

        using namespace rapidjson;

        const char* json = "{\"message\":\"orderbook\"}";
        Document rapidMeesage;
        rapidMeesage.Parse(json);
        
        rapidjson::Document::AllocatorType& allocator = rapidMeesage.GetAllocator();

        Value val(kObjectType);
        val.SetString(m_productId.c_str(), static_cast<SizeType>(m_productId.length()), allocator);
        rapidMeesage.AddMember("product_id",  val, allocator);

        Value jBids(kArrayType);
        Value jAsks(kArrayType);

        for(const auto& order: view->getAsks()){
            Value jOrder(kObjectType);

            val.SetString(order.order_id.c_str(), static_cast<SizeType>(order.order_id.length()), allocator);
            jOrder.AddMember("order_id", val, allocator);

            const std::string price = std::to_string(order.price);
            val.SetString(price.c_str(), static_cast<SizeType>(price.length()), allocator);
            jOrder.AddMember("price", val, allocator);

            const std::string size = std::to_string(order.size);
            val.SetString(size.c_str(), static_cast<SizeType>(size.length()), allocator);
            jOrder.AddMember("size", val, allocator);

            jAsks.PushBack(jOrder, allocator);
        }

        
        for(const auto& order: view->getBids()){
            Value jOrder(kObjectType);

            val.SetString(order.order_id.c_str(), static_cast<SizeType>(order.order_id.length()), allocator);
            jOrder.AddMember("order_id", val, allocator);

            const std::string price = std::to_string(order.price);
            val.SetString(price.c_str(), static_cast<SizeType>(price.length()), allocator);
            jOrder.AddMember("price", val, allocator);

            const std::string size = std::to_string(order.size);
            val.SetString(size.c_str(), static_cast<SizeType>(size.length()), allocator);
            jOrder.AddMember("size", val, allocator);

            jBids.PushBack(jOrder, allocator);
        }

        rapidMeesage.AddMember("bids", jBids, allocator);
        rapidMeesage.AddMember("asks", jAsks, allocator);

        std::chrono::steady_clock::time_point creating = std::chrono::steady_clock::now();
        auto differenceCreating = std::chrono::duration_cast<std::chrono::microseconds>(creating - begin).count();
        std::cout << "Publishing (creating json) took " << differenceCreating << " [µs]" << std::endl;

        rapidjson::StringBuffer strbuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        rapidMeesage.Accept(writer);

        std::string payload =  strbuf.GetString();

        std::chrono::steady_clock::time_point dumping = std::chrono::steady_clock::now();
        auto differenceDumping = std::chrono::duration_cast<std::chrono::microseconds>(dumping - begin).count();
        std::cout << "Publishing (dumping json) took " << differenceDumping << " [µs]" << std::endl;

        m_publisher.publish(payload);

        std::chrono::steady_clock::time_point sending = std::chrono::steady_clock::now();
        auto differenceSending = std::chrono::duration_cast<std::chrono::microseconds>(sending - dumping).count();
        std::cout << "Publishing (sending json) took " << differenceSending << " [µs]" << std::endl;
    }

    void publish(std::unique_ptr<Trade> trade){
        std::string payload = trade->generateMessage();

        // TODO begin return nlohmann json or create TradeView!
        auto jmessage = nlohmann::json::parse(payload); 
        jmessage["product_id"] = m_productId;
        payload = jmessage.dump();
        // TODO end
        m_publisher.publish(payload);
    }

    std::string m_productId;
    boost::circular_buffer<std::chrono::steady_clock::time_point> m_stamps;
    ZMQPublisher m_publisher;
};