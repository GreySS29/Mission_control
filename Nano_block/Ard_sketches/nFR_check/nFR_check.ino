#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h> 

#define CE_PIN  9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial) ; 

 
  printf_begin();

  Serial.println(F("nRF24 connection test"));
  Serial.println();

  if (!radio.begin()) {
    Serial.println(F("radio.begin() failed – check wiring and power!"));
    while (1);
  }

  
  radio.printDetails();  
  
}

void loop() {
}