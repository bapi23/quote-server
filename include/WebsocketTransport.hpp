#include <unordered_set>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_tls_client> websocket_client;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class WebsocketTransport{
public:
    WebsocketTransport(){
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_client.set_error_channels(websocketpp::log::elevel::all);

        m_client.init_asio();
        m_client.set_message_handler(bind(&WebsocketTransport::on_message, this, ::_1, ::_2));
        m_client.set_tls_init_handler([](websocketpp::connection_hdl) {
            return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
        });

        m_client.set_open_handler(bind(&WebsocketTransport::on_open, this, &m_client, ::_1));
    }

    ~WebsocketTransport(){
        m_thread->join();
    }

    void connect(const std::string& uri){
        websocketpp::lib::error_code ec;
        websocket_client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }

        m_client.connect(con);
        m_hdl =  con->get_handle();

        m_client.get_alog().write(websocketpp::log::alevel::app, "Connecting to " + uri);
        m_thread.reset(new websocketpp::lib::thread(&websocket_client::run, &m_client));
        sleep(1); // TODO fix with queuing messages to send!
    }

    void send(const std::string& msg){
        std::error_code ec;
        websocketpp::frame::opcode::value op;
        m_client.send(m_hdl, msg, websocketpp::frame::opcode::text, ec);
    }

    void subscribe(MessageReceiver* receiver){
        subscribers.insert(receiver);
    }

    void unsubscribe(MessageReceiver* receiver){
        subscribers.erase(subscribers);
    }

private:
    WebsocketTransport(WebsocketTransport& wt) = default;
    WebsocketTransport& operator=(const WebsocketTransport& wt) = default;

    void on_message(websocketpp::connection_hdl, websocket_client::message_ptr msg) {
        for(const auto& subscriber: subscribers){
            subscriber->onMessageReceived(msg->get_payload());
        }
    }

    void on_open(websocket_client* c, websocketpp::connection_hdl hdl) {
        std::cout << "Connection in open state" << std::endl;
    }

    websocket_client m_client;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::unordered_set<MessageReceiver*> subscribers; //TODO Introduce id?
};