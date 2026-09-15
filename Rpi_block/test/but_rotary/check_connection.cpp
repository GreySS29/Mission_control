#include <lgpio.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

const int BTN1_PIN = 17;
const int BTN2_PIN = 27;

const int ENC_CLK  = 5;
const int ENC_DT   = 6;
const int ENC_SW   = 13;

int chip_handle;
std::atomic<int> encoder_count{0};
int last_clk_level = 1;

const int ENC_MIN = -90;
const int ENC_MAX = 90;

void gpioCallback(int num_alerts, lgGpioAlert_p alerts, void *userdata) {
    for (int i = 0; i < num_alerts; i++) {
        int gpio  = alerts[i].report.gpio;
        int level = alerts[i].report.level;

        if (gpio == ENC_CLK) {
            int clk = level;
            int dt  = lgGpioRead(chip_handle, ENC_DT);

            if (clk != last_clk_level) {
                if (clk != dt) {
                    if (encoder_count < ENC_MAX) {
                        encoder_count++;
                        std::cout << "Encoder: " << encoder_count.load() << " (CW)\n";
                    } else {
                        std::cout << "Encoder: " << encoder_count.load() << " (max reached)\n";
                    }
                } else {
                    if (encoder_count > ENC_MIN) {
                        encoder_count--;
                        std::cout << "Encoder: " << encoder_count.load() << " (CCW)\n";
                    } else {
                        std::cout << "Encoder: " << encoder_count.load() << " (min reached)\n";
                    }
                }
            }
            last_clk_level = clk;
            continue;
        }

        if (level == 0) {
            std::cout << "Button on GPIO " << gpio << " pressed\n";
        }
    }
}

int main() {
    chip_handle = lgGpiochipOpen(0);
    if (chip_handle < 0) {
        std::cerr << "Failed to open gpiochip\n";
        return 1;
    }

    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, BTN1_PIN, -1);
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, BTN2_PIN, -1);
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, ENC_SW,  -1);
    lgGpioClaimInput(chip_handle, LG_SET_PULL_UP, ENC_DT);
    lgGpioClaimAlert(chip_handle, LG_SET_PULL_UP, LG_BOTH_EDGES, ENC_CLK, -1);

    lgGpioSetAlertsFunc(chip_handle, BTN1_PIN, gpioCallback, nullptr);
    lgGpioSetAlertsFunc(chip_handle, BTN2_PIN, gpioCallback, nullptr);
    lgGpioSetAlertsFunc(chip_handle, ENC_SW,  gpioCallback, nullptr);
    lgGpioSetAlertsFunc(chip_handle, ENC_CLK, gpioCallback, nullptr);

    std::cout << "Monitoring GPIO. Press Ctrl+C to exit.\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    lgGpiochipClose(chip_handle);
    return 0;
}
