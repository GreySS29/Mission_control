#pragma once

#include <boost/asio.hpp>
#include <string>

class WebSocketServer {
public:
    explicit WebSocketServer(boost::asio::io_context& io, unsigned short port);
    void run();

private:
    boost::asio::io_context& io_;
    unsigned short port_;
};

WebSocketServer::WebSocketServer(boost::asio::io_context& io, unsigned short port)
    : io_(io), port_(port) {
}

void WebSocketServer::run() {
    std::cout << "[ws] WebSocket server listening on port " << port_ << "\n";
    //  just prob
    
}
