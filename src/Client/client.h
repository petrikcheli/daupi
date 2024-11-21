#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include <queue>
#include <mutex>

using namespace boost::asio;

class Client
{
private:
    //ip::tcp::endpoint _ep_server{ip::address::from_string("127.0.0.1"), 8990};
    ip::tcp::socket _socket_server;
    static std::mutex queue_mutex_;

    ip::tcp::resolver _resolver;

private:
    void connect_to_server(const ip::tcp::resolver::results_type& endpoints);
    //void start_send_loop();

    void handle_write(const boost::system::error_code& error, std::size_t bytes_transferred);

    void receive_response();
public:
    void send_message(std::queue<std::shared_ptr<std::vector<unsigned char>>>& q_voice);
    boost::signals2::signal<void(std::shared_ptr<std::vector<unsigned char>>)> signal_voice_arrived;

    Client(boost::asio::io_context& io_context, const std::string& host, int port)
        : _socket_server(io_context), _resolver(io_context) {
        auto endpoints = _resolver.resolve(host, std::to_string(port));
        connect_to_server(endpoints);
    }
};
