#define ENCODER_CLK 14  // D5
#define ENCODER_DT  12  // D6

int lastClkState = HIGH;
long counter = 0;

const int ENC_MIN = -90;
const int ENC_MAX = 90;

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT,  INPUT);
}

void loop() {
  int clk = digitalRead(ENCODER_CLK);

  if (clk != lastClkState) {
    if (clk == LOW) {
      int dt = digitalRead(ENCODER_DT);
      if (dt == HIGH) {
        if(counter < ENC_MAX) {
        counter+=5;
        Serial.print("CW, count = ");
        }
      } else {
        if(counter > ENC_MIN){
        counter-=5;
        Serial.print("CCW, count = ");
        }
        
      }
      Serial.println(counter);
    }
    lastClkState = clk;
  }
}
