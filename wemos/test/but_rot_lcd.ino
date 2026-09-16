#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_CLK 14  
#define ENCODER_DT  12  

#define BTN_1  0  // D3 = GPIO0
#define BTN_2  2  // D4 = GPIO2

LiquidCrystal_I2C lcd(0x27, 16, 2);  
int lastClkState = HIGH;
long angle = 0;

const int ENC_MIN = -180;
const int ENC_MAX = 180;


unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

int lastBtn1State = HIGH;
int lastBtn2State = HIGH;

void updateAngleDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Angle: ");
  lcd.print(angle);
  lcd.print("    ");
}

void showButtonMessage(const char* msg) {
  lcd.setCursor(0, 1);
  lcd.print(msg);
  int len = strlen(msg);
  for (int i = len; i < 16; i++) {
    lcd.print(' ');
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT,  INPUT);

  pinMode(BTN_1, INPUT_PULLUP); 
  pinMode(BTN_2, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  updateAngleDisplay();
}

void loop() {
  int clk = digitalRead(ENCODER_CLK);

  if (clk != lastClkState) {
    if (clk == LOW) {
      int dt = digitalRead(ENCODER_DT);
      if (dt == HIGH) {
        if (angle < ENC_MAX) {
          angle += 5;
        }
      } else {
        if (angle > ENC_MIN) {
          angle -= 5;
        }
      }
      Serial.print("angle = ");
      Serial.println(angle);
      updateAngleDisplay();
    }
    lastClkState = clk;
  }


  int btn1 = digitalRead(BTN_1);
  int btn2 = digitalRead(BTN_2);

  if (btn1 != lastBtn1State || btn2 != lastBtn2State) {
    lastDebounceTime = millis();
  }
  lastBtn1State = btn1;
  lastBtn2State = btn2;

  if (millis() - lastDebounceTime > debounceDelay) {
    static int lastStableBtn1 = HIGH;
    static int lastStableBtn2 = HIGH;

    // Button 1 pressed
    if (btn1 == LOW && lastStableBtn1 == HIGH) {
      Serial.println("Button 1 (D1) pressed");
      showButtonMessage("Button 1 pressed");
    }

    // Button 2 pressed
    if (btn2 == LOW && lastStableBtn2 == HIGH) {
      Serial.println("Button 2 (D2) pressed");
      showButtonMessage("Button 2 pressed");
    }

    lastStableBtn1 = btn1;
    lastStableBtn2 = btn2;
  }
}
