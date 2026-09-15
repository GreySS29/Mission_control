#include <lgpio.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

// BCM pin numbers
const int BTN1_PIN = 17;
const int BTN2_PIN = 27;

// Rotary encoder
const int ENC_CLK  = 5;
const int ENC_DT   = 6;
const int ENC_SW   = 13;

int chip_handle;
std::atomic<int> encoder_count{0};
int last_clk_level = 1;

void gpioCallback(int chip, int gpio, int level, uint64_t tick, void *userdata) {
    if (gpio == ENC_CLK) {
        int clk = level;
        int dt  = lgGpioRead(chip_handle, ENC_DT);

        if (clk != last_clk_level) {
            if (clk != dt) {
                encoder_count++;
                std::cout << "Encoder: " << encoder_count.load() << " (CW)\n";
            } else {
                encoder_count--;
                std::cout << "Encoder: " << encoder_count.load() << " (CCW)\n";
            }
        }
        last_clk_level = clk;
        return;
    }

    // Buttons (и SW энкодера)
    if (level == 0) {
        std::cout << "Button on GPIO " << gpio << " pressed\n";
    }
}

int main() {
    chip_handle = lgGpiochipOpen(0);
    if (chip_handle < 0) {
        std::cerr << "Failed to open gpiochip\n";
        return 1;
    }

    // Buttons
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, BTN1_PIN, -1);
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, BTN2_PIN, -1);
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, ENC_SW,  -1);

    // Encoder DT — просто вход, читаем по запросу
    lgGpioClaimInput(chip_handle, LG_SET_PULL_UP, ENC_DT);

    // Encoder CLK — с алертом
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, ENC_CLK, -1);

    // Единый callback на все линии этого чипа
    lgGpioSetSamplesFunc(gpioCallback, nullptr);

    std::cout << "Monitoring GPIO. Press Ctrl+C to exit.\n";
    std::cout << "Buttons: GPIO " << BTN1_PIN << ", " << BTN2_PIN << "\n";
    std::cout << "Encoder: CLK=" << ENC_CLK << ", DT=" << ENC_DT << ", SW=" << ENC_SW << "\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    lgGpiochipClose(chip_handle);
    return 0;
}
