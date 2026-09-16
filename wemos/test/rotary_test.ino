#define ENCODER_CLK 14 
#define ENCODER_DT  12  

int lastClkState = HIGH;
long angle = 0;

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
        if(angle < ENC_MAX) {
        angle+=5;
        Serial.print("angle = ");
        }
      } else {
        if(angle > ENC_MIN){
        angle-=5;
        Serial.print("angle = ");
        }
        
      }
      Serial.println(angle);
    }
    lastClkState = clk;
  }
}
