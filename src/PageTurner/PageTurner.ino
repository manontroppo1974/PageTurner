/**
 * This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
 */
#include <BleKeyboard.h>
#include <BleMouse.h>

#define PIN_LED0 2
#define PIN_BTN0 4
#define PIN_BTN1 5

BleKeyboard bleKeyboard;
BleMouse bleMouse;

int gConnect = 0;
int gBtn0 = HIGH;
int gBtn1 = HIGH;
int gDevType = 0;   // 0:Keyboard, 1:Mouse
int gRepeat = 0;
int gBlinkFlg = 0;

uint8_t chrBtn0 = 'b';
uint8_t chrBtn1 = 'f';

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED0, OUTPUT);
  pinMode(PIN_BTN0, INPUT_PULLUP);
  pinMode(PIN_BTN1, INPUT_PULLUP);

  int btn0 = digitalRead(PIN_BTN0);
  int btn1 = digitalRead(PIN_BTN1);

  if(btn0 == LOW){
    Serial.println("Starting BLE Mouse");
    gDevType = 1;
    bleMouse.begin();
  }
  else{
    Serial.println("Starting BLE Keyboard");
    gDevType = 0;
    bleKeyboard.begin();
  }

  if(btn1 == LOW){
    gRepeat = 1;
  }
  else{
    gRepeat = 0;
  }
}

void loop() {
  int btn0 = digitalRead(PIN_BTN0);
  int btn1 = digitalRead(PIN_BTN1);
  int delayTime = 100;
  
  if((gDevType == 0 && bleKeyboard.isConnected()) || 
     (gDevType == 1 && bleMouse.isConnected())) {
    if(gConnect == 0){
      gConnect = 1;
      digitalWrite(PIN_LED0, HIGH);
      Serial.println("BLE: connected");
    }

    int keyFlg = 0;
    if(gBtn0 != btn0 || gRepeat){
      if(btn0 == LOW){
        keyFlg = 1;
        Serial.println("Button 0");
        if(gDevType == 0){
          bleKeyboard.write(chrBtn0);
        }
        else{
          bleMouse.move(0,0,1);   // Scroll Up
        }
        gBlinkFlg = 1;
      }      
    }    
    if(gBtn1 != btn1 || gRepeat){
      if(btn1 == LOW && keyFlg == 0){
        keyFlg = 1;
        Serial.println("Button 1");
        if(gDevType == 0){
          bleKeyboard.write(chrBtn1);
        }
        else{
          bleMouse.move(0,0,-1);  // Scroll Down
        }
        gBlinkFlg = 1;
      }      
    }
    if(keyFlg == 0){
      if(gBlinkFlg == 1){
        gBlinkFlg = -1;
      }
    }
  }
  else {
    if(gConnect == 1){
      gConnect = 0;
      digitalWrite(PIN_LED0, LOW);
      Serial.println("BLE: disconnected");
    }
    delayTime = 1000;
  }
  
  gBtn0 = btn0;
  gBtn1 = btn1;

  if(gBlinkFlg == 1){
    int led0Val = digitalRead(PIN_LED0);
    led0Val = (led0Val + 1) % 2;
    digitalWrite(PIN_LED0, led0Val);
  }else if(gBlinkFlg == -1){
    digitalWrite(PIN_LED0, HIGH);
    gBlinkFlg = 0;
  }
  
  delay(delayTime);
}
