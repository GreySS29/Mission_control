#include <RF24/RF24.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#define CE_PIN  22
#define CSN_PIN 1

RF24 radio(CE_PIN, CSN_PIN);

const uint8_t address[5] = "00001";

int main() {
  if (!radio.begin()) {
    fprintf(stderr, "radio.begin() failed\n");
    return 1;
  }

  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setAddressWidth(5);
  radio.setAutoAck(true);
  radio.setRetries(5, 5);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();

  radio.openReadingPipe(0, address);
  radio.startListening();

  printf("Pi ping receiver ready\n");

  while (true) {
    if (radio.available()) {
      char buf[32] = {0};
      radio.read(buf, sizeof(buf));

      printf("Received: %s\n", buf);

     
      const char pong[] = "PONG";
      radio.writeAckPayload(0, pong, sizeof(pong));
    }

    usleep(10000);  
  }

  return 0;
}