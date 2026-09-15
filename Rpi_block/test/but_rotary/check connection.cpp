#include <pigpio.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

// BCM pin numbers
const int BTN1_PIN = 17;
const int BTN2_PIN = 27;
const int ENC_CLK  = 22;
const int ENC_DT   = 23;
const int ENC_SW   = 24;

std::atomic<int> encoder_count{0};
int last_clk_level = 1;

void buttonCallback(int gpio, int level, int tick) {
    // level == 0 => pressed (falling edge with pull-up)
    if (level == 0) {
        std::cout << "Button on GPIO " << gpio << " pressed\n";
    }
}

void encoderCallback(int gpio, int level, int tick) {
    if (gpio != ENC_CLK) return;

    int clk = level;          // current CLK level (0 or 1)
    int dt  = gpioRead(ENC_DT);

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
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio\n";
        return 1;
    }

    // Set pins as inputs with pull-up
    gpioSetMode(BTN1_PIN, PI_INPUT);
    gpioSetPullUpDown(BTN1_PIN, PI_PUD_UP);

    gpioSetMode(BTN2_PIN, PI_INPUT);
    gpioSetPullUpDown(BTN2_PIN, PI_PUD_UP);

    gpioSetMode(ENC_CLK, PI_INPUT);
    gpioSetPullUpDown(ENC_CLK, PI_PUD_UP);

    gpioSetMode(ENC_DT, PI_INPUT);
    gpioSetPullUpDown(ENC_DT, PI_PUD_UP);

    gpioSetMode(ENC_SW, PI_INPUT);
    gpioSetPullUpDown(ENC_SW, PI_PUD_UP);

    // Register callbacks for falling edges (button press / encoder switch)
    gpioSetAlertFunc(BTN1_PIN, buttonCallback);
    gpioSetAlertFunc(BTN2_PIN, buttonCallback);
    gpioSetAlertFunc(ENC_SW,  buttonCallback);

    // For encoder rotation, we monitor CLK changes
    gpioSetAlertFunc(ENC_CLK, encoderCallback);

    std::cout << "Monitoring GPIO. Press Ctrl+C to exit.\n";
    std::cout << "Buttons: GPIO " << BTN1_PIN << ", " << BTN2_PIN << "\n";
    std::cout << "Encoder: CLK=" << ENC_CLK << ", DT=" << ENC_DT << ", SW=" << ENC_SW << "\n";

    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gpioTerminate();
    return 0;
}
