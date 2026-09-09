
#include <iostream>
#include <chrono>
#include <thread>
#include "app_state.h"
#include "websocket_server.hpp"
#include <boost/asio.hpp>

int main() {
    std::cout << "[main] RPi data hub started\n";

    AppState state;
    boost::asio::io_context io;

    WebSocketServer ws(io,8080);
    ws.ru()

    
    for (int i = 0; i < 10; ++i) {
        temperature += 0.5;
        state.setTemperature(temperature);
    

   nlohmann::json j = state.snapshot();
        std::cout << "[main] tick " << i
                  << " temperature_c=" << j["temperature_c"] << "\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[main] RPi data hub stopped\n";
    return 0;
}
