#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

namespace Config {

//nRF24
constexpr uint8_t RadioCePin = 9;
constexpr uint8_t RadioCsnPin = 10;

//LCD
constexpr uint8_t LcdAddress = 0x27;
constexpr uint8_t LcdColumns = 16;
constexpr uint8_t LcdRows = 2;

constexpr uint8_t RadioChannel = 100;
constexpr uint8_t RadioAddressWidth = 5;
constexpr uint8_t ReadingPipe = 1;

//Buffer
constexpr unsigned long SerialBaudRate = 115200;
constexpr unsigned long SendIntervalMs = 1000; // it will  be 50 ms
constexpr unsigned long ResponseTimeoutMs = 500; // check it
constexpr uint8_t MaxMessageLength = 32;

// Arduino -> Raspberry Pi
constexpr char PiAddress[] = "PI001";

// Raspberry Pi -> Arduino
constexpr char ArduinoAddress[] = "AR001";
}

RF24 radio(
  Config::RadioCePin,
  Config::RadioCsnPin
);

LiquidCrystal_I2C lcd(
  Config::LcdAddress,
  Config::LcdColumns,
  Config::LcdRows
);

uint32_t packetCounter = 0;

void displayLine(uint8_t row, const char* text) {
  lcd.setCursor(0, row);
  for (uint8_t column = 0; column < Config::LcdColumns; ++column) {
    if (text[column] == '\0') {
      lcd.print(' ');
    } else {
      lcd.print(text[column]);
    }
  }
}

void displayMessage(
  const char* firstLine,
  const char* secondLine
) {
  displayLine(0, firstLine);
  displayLine(1, secondLine);
}

//check this part
void showRadioError() {
  Serial.println(F("nRF24 initialization failed"));

  displayMessage(
    "nRF24 FAILED",
    "Check power"
  );

  while (true) {
    // Stop execution because the radio is unavailable.
  }
}

void configureRadio() {
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(Config::RadioChannel);
  radio.setAddressWidth(Config::RadioAddressWidth);

  radio.setAutoAck(true);
  radio.setRetries(5, 5);

  radio.enableDynamicPayloads();

  radio.openWritingPipe(Config::PiAddress);

  radio.openReadingPipe(
    Config::ReadingPipe,
    Config::ArduinoAddress
  );

  
  radio.startListening();
}

// RocketSim ----> arduino !! rewrite
void createPacket(
  char* message,
  size_t messageSize
) {
  snprintf(
    message,
    messageSize,
    "Rocket_Param %lu",
    static_cast<unsigned long>(packetCounter)
  );
}

bool sendPacket(const char* message) {
  radio.stopListening();

  const bool sentSuccessfully = radio.write(
    message,
    strlen(message) + 1
  );

  Serial.print(F("Sent: "));
  Serial.print(message);
  Serial.print(F(" | ACK: "));
  Serial.println(
    sentSuccessfully ? F("OK") : F("FAIL")
  );

  return sentSuccessfully;
}

// next it should send data ----> RocketSim
bool receiveResponse() {
  radio.startListening();

  const unsigned long startTime = millis();

  while (millis() - startTime < Config::ResponseTimeoutMs) {
    if (!radio.available()) {
      continue;
    }

    char response[Config::MaxMessageLength] = {};

    radio.read(
      response,
      sizeof(response)
    );

    Serial.print(F("Received: "));
    Serial.println(response);

    displayMessage(
      "Response received",
      response
    );

    return true;
  }

  Serial.println(F("No response received"));

  displayMessage(
    "No response",
    "from Pi"
  );

  return false;
}

void sendAndReceive() {
  char message[Config::MaxMessageLength] = {};

  createPacket(
    message,
    sizeof(message)
  );

  displayMessage(  // it is not nessesary
    "Sending...",
    message
  );

  const bool sentSuccessfully = sendPacket(message);

  if (!sentSuccessfully) {
    displayMessage(
      "Send FAILED",
      "Pi not replying"
    );

    return;
  }

  displayMessage(
    "Sent OK",
    "Waiting..."
  );

  receiveResponse();
}

void setup() {
  Serial.begin(Config::SerialBaudRate);

  lcd.init();
  lcd.backlight();

  displayMessage(
    "nRF24 TX/RX Nano",
    "Starting..."
  );

  if (!radio.begin()) {
    showRadioError();
  }

  configureRadio();

  displayMessage(
    "nRF24 ready",
    "Send/receive"
  );

  delay(1000);
}

void loop() {
  sendAndReceive();

  ++packetCounter;

  delay(Config::SendIntervalMs);
}
