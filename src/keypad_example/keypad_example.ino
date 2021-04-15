#include "Arduino.h"
#include "hidkeyboard.h"
#include "Keypad.h"

const byte ROWS = 3; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','4','7','#'},
  {'2','5','8','0'},
  {'3','6','9','*'}
};

byte rowPins[ROWS] = {10, 11, 12}; //connect to the row pinouts of the kpd
//byte colPins[COLS] = {23, 24, 25, 26}; //connect to the column pinouts of the kpd
byte colPins[COLS] = {1, 2, 3, 4}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;

HIDkeyboard dev;

class MyHIDCallbacks: public HIDCallbacks{
  void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);  
     for (size_t i = 0; i < bufsize; i++)
    {
        Serial.printf("%d\n", buffer[i]);
    }   
  }
};

void dataCB(uint8_t report_id, uint8_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    for (size_t i = 0; i < bufsize; i++)
    {
        Serial.printf("%d\n", buffer[i]);
        Serial.printf("%c\n", buffer[i]);
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  loopCount = 0;
  startTime = millis();
  msg = "";
  dev.begin();
  dev.setCallbacks(new MyHIDCallbacks());
}

void loop() {
  /*if ( (millis()-startTime)>5000 ) {
    Serial.print("Average loops per second = ");
    Serial.println(loopCount/5);
    startTime = millis();
    loopCount = 0;
  }*/
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
          Serial.print("Wi ");
          dev.sendString(String(kpd.key[i].kchar));
          Serial.print("Non ");
          msg = " PRESSED.";
        break;
          case HOLD:
          dev.sendString(String(kpd.key[i].kchar));
          msg = " HOLD.";
        break;
          case RELEASED:
          msg = " RELEASED.";
        break;
          case IDLE:
          msg = " IDLE.";
          
        }
        Serial.print("Key ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
      }
    }
  }
}
