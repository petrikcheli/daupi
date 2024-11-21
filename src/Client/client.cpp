#include "client.h"
#include "../Audio/Audio_parametrs.h"
#include <iostream>

void Client::connect_to_server(const ip::tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(_socket_server, endpoints,
                                [this, endpoints](const boost::system::error_code& error, const ip::tcp::endpoint&) {
                                    if (!error) {
                                        std::cout << "Connected to server." << std::endl;
                                        this->receive_response();
                                    } else {
                                        std::cout << "Failed connect to server" << std::endl;
                                    }
                                    //this->connect_to_server(endpoints);
                                });
}

void Client::handle_write(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::cout << "done message!! " << bytes_transferred << " byte\n";
    } else {
        std::cerr << "error send message : " << error.message() << "\n";
    }
}

void Client::send_message(std::queue<std::shared_ptr<std::vector<unsigned char>>>& q_voice) {
    std::vector<unsigned char> data_send = *q_voice.front();

    _socket_server.async_send(boost::asio::buffer(data_send),
                            [this, &q_voice](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                q_voice.pop();
                                handle_write(error, bytes_transferred);
                            });
}


void Client::receive_response() {
    auto buffer = std::make_shared<std::vector<unsigned char>>(daupi::FRAMES_PER_BUFFER*daupi::CHANNELS*sizeof(float));
    _socket_server.async_receive(boost::asio::buffer(*buffer),
                                [this, buffer](const boost::system::error_code& error, std::size_t bytes_transferred){
                                    if(!error){
                                        buffer->resize(bytes_transferred);
                                        this->signal_voice_arrived(buffer);
                                        this->receive_response();
                                    } else {
                                        std::cout << "error receive message" << std::endl;
                                    }
                                });
}
