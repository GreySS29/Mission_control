
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "[main] RPi data hub started\n";

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[main] tick " << i << "\n";
    }

    std::cout << "[main] RPi data hub stopped\n";
    return 0;
}