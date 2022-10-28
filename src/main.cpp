// COMP-10184 - Mohawk College
// Project Alarm System
// 
// Rudimentary PIR based silent alarm
// 
// @author    Douglas Ward
// @id        000333587
//
// I created this work and I have not shared it with anyone else.
// 

#include <Arduino.h> 
 
// digital input pin definitions 
#define PIN_PIR D5 
#define PIN_BUTTON D6

// Alarm states
#define ALARM_DISABLED 0
#define ALARM_ENABLED 1
#define ALARM_COUNTDOWN 2
#define ALARM_ACTIVE 3

// Button delay
#define BUTTON_DELAY 2000
#define STATE_CHANGE_DELAY 2500

// Alarm countdown
#define ALARM_DELAY 10000

// Global button state information
bool bButton;
bool bMotion;
int iAlarmState;
int iAlarmLastState;
int iCountDown;

// *************************************************************
// handles button and pir reading
//  
void checkInputs() {
  // read digital value from button
  bButton = !digitalRead(PIN_BUTTON);

  if(bButton) {
    delay(BUTTON_DELAY);
  }

  //read motion sensor
  bMotion = digitalRead(PIN_PIR);
}

// ************************************************************* 
void setup() { 
  // configure the USB serial monitor 
  Serial.begin(115200); 
 
  // configure the LED output 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
 
  // PIR sensor is an INPUT 
  pinMode(PIN_PIR, INPUT); 
 
  // Button is an INPUT 
  pinMode(PIN_BUTTON, INPUT_PULLUP); 

  iAlarmState = ALARM_DISABLED;
  iCountDown = millis();
}

// *************************************************************
// Blinks the builtin LED on and off 4 times
void doLEDLoop() {
  int iBlink = int(millis()) % 1000;
  if(iBlink >= 1 && iBlink <= 250) {
    digitalWrite(LED_BUILTIN,LOW);
  } else if(iBlink >= 251 && iBlink <= 500) {
    digitalWrite(LED_BUILTIN,HIGH);
  } else if(iBlink >= 501 && iBlink <= 750) {
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    digitalWrite(LED_BUILTIN,HIGH);
  }
}
 
// ************************************************************* 
void loop() {
  checkInputs();
  
  if(bButton) {
    digitalWrite(LED_BUILTIN,HIGH); // all button presses turn off the light
    switch(iAlarmState) {
      case ALARM_DISABLED:
        iAlarmState = ALARM_ENABLED;
        delay(2500); // pir sensor picks up your hand pushing the button
        break;
      case ALARM_ENABLED: // should be impossible to hit the button without triggering state change
        break;
      case ALARM_COUNTDOWN:
        iAlarmState = ALARM_DISABLED;
        break;
      case ALARM_ACTIVE:
        iAlarmState = ALARM_DISABLED;
        break;
    }
  }  else {

    if(bMotion) {
     switch(iAlarmState) {
        case ALARM_DISABLED:
          break;
        case ALARM_ACTIVE:
          break;
        case ALARM_COUNTDOWN:
          break;
        case ALARM_ENABLED:
          iAlarmState = ALARM_COUNTDOWN;
          iCountDown = millis();
          break;
      }
    }
  }

  if(iAlarmState == ALARM_COUNTDOWN) {
    doLEDLoop();
    if(millis() - iCountDown >= 10000) {
      iAlarmState = ALARM_ACTIVE;
    }
  }

  if(iAlarmState == ALARM_ACTIVE) {
    digitalWrite(LED_BUILTIN,LOW);
  }

  if(iAlarmState != iAlarmLastState) {
    Serial.printf("%d",iAlarmState);
  }

  iAlarmLastState = iAlarmState;
}