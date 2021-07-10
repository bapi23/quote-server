#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using json = nlohmann::json;

auto subscribe_test = json::parse(R"({
    "type": "subscribe",
    "product_ids": [
        "ETH-USD",
        "ETH-EUR"
    ],
    "channels": [
        "level2",
        "heartbeat",
        {
            "name": "ticker",
            "product_ids": [
                "ETH-BTC",
                "ETH-USD"
            ]
        }
    ]
})");

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "OPEN" << std::endl;
    std::error_code ec;
    websocketpp::frame::opcode::value op;
    c->send(hdl, subscribe_test.dump(), websocketpp::frame::opcode::text, ec);
    std::cout << "ec: " << ec;
}


int main(int argc, char* argv[]) {
    //snapshot request:
    // RAII cleanup

    curlpp::Cleanup myCleanup;

    // Send request and get a result.
    // Here I use a shortcut to get it in a string stream ...

    std::ostringstream os;
    os << curlpp::options::Url(std::string("http://example.com GET /products/ETH-BTC/book"));

    std::string asAskedInQuestion = os.str();
    std::cout << asAskedInQuestion;

    client c;

    std::string hostname = "ws-feed.pro.coinbase.com";
    std::string port = "9002";

    std::string uri = "wss://ws-feed.pro.coinbase.com";

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_error_channels(websocketpp::log::elevel::all);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(&on_message);
        c.set_tls_init_handler([](websocketpp::connection_hdl) {
            return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
        });
        c.set_open_handler(bind(&on_open, &c, ::_1));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        c.get_alog().write(websocketpp::log::alevel::app, "Connecting to " + uri);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}
