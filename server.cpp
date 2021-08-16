// #include <websocketpp/config/asio_client.hpp>
// #include <websocketpp/client.hpp>
// #include <nlohmann/json.hpp>
// #include <iostream>

// #include <curlpp/cURLpp.hpp>
// #include <curlpp/Easy.hpp>
// #include <curlpp/Options.hpp>
// #include <curlpp/Exception.hpp>

// typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
// typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

// using websocketpp::lib::placeholders::_1;
// using websocketpp::lib::placeholders::_2;
// using websocketpp::lib::bind;
// using json = nlohmann::json;

// auto subscribe_test = json::parse(R"({
//     "type": "subscribe",
//     "product_ids": [
//         "ETH-USD",
//         "ETH-EUR"
//     ],
//     "channels": [
//         "full",
//         "heartbeat",
//         {
//             "name": "ticker",
//             "product_ids": [
//                 "ETH-BTC",
//                 "ETH-USD"
//             ]
//         }
//     ]
// })");

// void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
//     std::cout << msg->get_payload() << std::endl;
// }

// void on_open(client* c, websocketpp::connection_hdl hdl) {
//     std::cout << "OPEN" << std::endl;
//     std::error_code ec;
//     websocketpp::frame::opcode::value op;
//     c->send(hdl, subscribe_test.dump(), websocketpp::frame::opcode::text, ec);
//     std::cout << "ec: " << ec;
// }


// int main(int argc, char* argv[]) {
//     //snapshot request:
//     // RAII cleanup
//     try {
//         curlpp::Cleanup myCleanup;
//         curlpp::Easy request;
        
//         request.setOpt(new curlpp::options::Url(std::string("https://api-public.sandbox.pro.coinbase.com/products/ETH-BTC/book?level=3")));
//         request.setOpt(new curlpp::options::UserAgent("Chrome: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36."));

//         std::cout << request;
//     }
//     catch ( curlpp::LogicError & e ) {
//         std::cout << e.what() << std::endl;
//     }
//     catch ( curlpp::RuntimeError & e ) {
//         std::cout << e.what() << std::endl;
//     }


//     client c;
//     std::string uri = "wss://ws-feed.pro.coinbase.com";

//     try {
//         // Set logging to be pretty verbose (everything except message payloads)
//         c.set_access_channels(websocketpp::log::alevel::all);
//         c.clear_access_channels(websocketpp::log::alevel::frame_payload);
//         c.set_error_channels(websocketpp::log::elevel::all);

//         // Initialize ASIO
//         c.init_asio();

//         // Register our message handler
//         c.set_message_handler(&on_message);
//         c.set_tls_init_handler([](websocketpp::connection_hdl) {
//             return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
//         });
//         c.set_open_handler(bind(&on_open, &c, ::_1));

//         websocketpp::lib::error_code ec;
//         client::connection_ptr con = c.get_connection(uri, ec);
//         if (ec) {
//             std::cout << "could not create connection because: " << ec.message() << std::endl;
//             return 0;
//         }

//         // Note that connect here only requests a connection. No network messages are
//         // exchanged until the event loop starts running in the next line.
//         c.connect(con);

//         c.get_alog().write(websocketpp::log::alevel::app, "Connecting to " + uri);

//         // Start the ASIO io_service run loop
//         // this will cause a single connection to be made to the server. c.run()
//         // will exit when this connection is closed.
//         c.run();
//     } catch (websocketpp::exception const & e) {
//         std::cout << e.what() << std::endl;
//     }
// }

#include "coinbase/CoinbaseFeedClient.hpp"
#include "Market.hpp"
#include "ClientRegister.hpp"
#include "product/ProductChangePublisherFactoryZMQ.hpp"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    using namespace std::chrono_literals;
    auto feed = std::make_unique<CoinbaseFeedClient>();
    std::unique_ptr<ProductChangePublisherFactory> publisherFactory = std::make_unique<ProductChangePublisherFactoryZMQ>();
    std::unique_ptr<Market> market = std::make_unique<Market>(std::move(feed), std::move(publisherFactory));
    ClientRegister clientReg(market.get(), "127.0.0.1");
    clientReg.run();
    bool done = false;

    // auto response = RestTransport::request("https://api.pro.coinbase.com/products/ETH-BTC/book?level=3");
    // WebsocketTransport transport;
    // transport.connect("wss://ws-feed.pro.coinbase.com");
    // sleep(1);
    // transport.send(subscribe_test.dump());
    //market.subscribe("clientId1", "ETH-USD");
    while(!done){
        std::this_thread::sleep_for(20ms);
    }
}
