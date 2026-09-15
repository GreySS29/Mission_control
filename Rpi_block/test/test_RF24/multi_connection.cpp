    #include <RF24/RF24.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

namespace Config {
//nRF24
constexpr uint8_t CePin = 22;
constexpr uint8_t CsnPin = 1;

constexpr uint8_t RadioChannel = 100;
constexpr uint8_t RadioAddressWidth = 5;
constexpr uint8_t ReadingPipe = 1;

constexpr unsigned long PollIntervalMs = 10;
constexpr unsigned long ResponseDelayMs = 10;

constexpr size_t MaxMessageLength = 32;

// Raspberry Pi -> Arduino
constexpr char ArduinoAddress[] = "AR001";

// Arduino -> Raspberry Pi
constexpr char PiAddress[] = "PI001";
}

RF24 radio(
  Config::CePin,
  Config::CsnPin
);

void configureRadio() {
  radio.setChannel(Config::RadioChannel);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setAddressWidth(Config::RadioAddressWidth);

  radio.setAutoAck(true);
  radio.setRetries(5, 5);

  radio.enableDynamicPayloads();

  // Raspberry Pi sends responses to the Arduino.
  radio.openWritingPipe(
    Config::ArduinoAddress
  );

  // Raspberry Pi receives packets from the Arduino.
  radio.openReadingPipe(
    Config::ReadingPipe,
    Config::PiAddress
  );

  radio.startListening();
}

bool receivePacket(char* message, size_t messageSize) {
  if (!radio.available()) {
    return false;
  }

  std::memset(
    message,
    0,
    messageSize
  );

  radio.read(
    message,
    messageSize
  );

  return true;
}

bool sendResponse(const char* response) {
  radio.stopListening();

  const bool sentSuccessfully = radio.write(
    response,
    std::strlen(response) + 1
  );

  radio.startListening();

  return sentSuccessfully;
}

int main() {
  if (!radio.begin()) {
    std::fprintf(
      stderr,
      "radio.begin() failed\n"
    );

    return 1;
  }

  configureRadio();

  std::printf(
    "Raspberry Pi nRF24 receiver/transmitter ready\n"
  );

  std::printf(
    "Waiting for messages from Arduino...\n"
  );

  while (true) {
    char receivedMessage[Config::MaxMessageLength] = {};

    if (receivePacket(
          receivedMessage,
          sizeof(receivedMessage)
        )) {
      std::printf(
        "Received: %s\n",
        receivedMessage
      );

      std::this_thread::sleep_for(
        std::chrono::milliseconds(
          Config::ResponseDelayMs
        )
      );

      char response[Config::MaxMessageLength] = {};

      std::snprintf(
        response,
        sizeof(response),
        "Pi ACK: %.24s",
        receivedMessage
      );

      const bool responseSent = sendResponse(response);

      std::printf(
        "Response: %s | ACK: %s\n",
        response,
        responseSent ? "OK" : "FAIL"
      );
    }

    std::this_thread::sleep_for(
      std::chrono::milliseconds(
        Config::PollIntervalMs
      )
    );
  }

  return 0;
}
