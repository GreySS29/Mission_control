
#include <iostream>
#include <chrono>
#include <thread>
#include "app_state.h"
#include "../include/websocket_server.h"
#include <boost/asio.hpp>

int main() {
    std::cout << "[main] RPi data hub started\n";

    AppState state;
    boost::asio::io_context io;

    WebSocketServer ws(io,8080);
    ws.run();

   
    for (int i = 0; i < 10; ++i) {
        state.setTemperature(state.get_temperature() + 0.5);
    

   nlohmann::json j = state.snapshot();
        std::cout << "[main] tick " << i
                  << " temperature_c=" << j["temperature_c"] << "\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[main] RPi data hub stopped\n";
    return 0;
}
