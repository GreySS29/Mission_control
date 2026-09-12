#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

constexpr uint8_t NRF_CE_PIN = 9;
constexpr uint8_t NRF_CSN_PIN = 10;

constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;

constexpr uint8_t RADIO_CHANNEL = 100;   

const byte RADIO_ADDRESS[6] = "00001";

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

LiquidCrystal_I2C lcd(
  LCD_ADDRESS,
  LCD_COLUMNS,
  LCD_ROWS
);

uint32_t packetCounter = 0;

void printLine(uint8_t row, const char* text) {
  lcd.setCursor(0, row);

  for (uint8_t i = 0; i < LCD_COLUMNS; ++i) {
    if (text[i] != '\0') {
      lcd.print(text[i]);
    } else {
      lcd.print(' ');
    }
  }
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  printLine(0, "nRF24 TX Nano");
  printLine(1, "Starting...");

  if (!radio.begin()) {
    Serial.println(F("nRF24 FAILED"));
    printLine(0, "nRF24 FAILED");
    printLine(1, "Check power");

    while (true) {
    }
  }

  radio.setPALevel(RF24_PA_MIN);        
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RADIO_CHANNEL);
  radio.setAddressWidth(5);
  radio.setAutoAck(true);
  radio.setRetries(5, 5);
  radio.enableDynamicPayloads();        
  radio.enableAckPayload();             

  radio.openWritingPipe(RADIO_ADDRESS);
  radio.stopListening();

  printLine(0, "nRF24 TX ready");
  printLine(1, "Sending...");
}

void loop() {
  char message[32];

  snprintf(
    message,
    sizeof(message),
    "Rock_Param %lu",
    static_cast<unsigned long>(packetCounter)
  );

  bool sent = radio.write(message, strlen(message) + 1);

  Serial.print(F("Sent: "));
  Serial.print(message);
  Serial.print(F(" | ACK: "));
  Serial.println(sent ? F("OK") : F("FAIL"));

  if (sent) {
    printLine(0, "Sent OK");
    printLine(1, message);

   
    if (radio.isAckPayloadAvailable()) {
      char ackBuf[32] = {0};
      radio.read(ackBuf, sizeof(ackBuf));

      Serial.print(F("ACK payload: "));
      Serial.println(ackBuf);

      printLine(1, ackBuf);
    }
  } else {
    printLine(0, "Send FAILED");
    printLine(1, "Pi not replying");
  }

  ++packetCounter;
  delay(1000);
}