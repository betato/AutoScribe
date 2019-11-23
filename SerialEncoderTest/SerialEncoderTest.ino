#pragma once

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

const char *ssid = "DeliciousSuspicious";
const char *pass =  "chamfers";

#define ENCODER_L_PHASE_A 4
#define ENCODER_L_PHASE_B 5

#define ENCODER_R_PHASE_A 12
#define ENCODER_R_PHASE_B 14

#define BUTTON_PIN 16
#define DEBOUNCE_DELAY 50

void setup() {
  Serial.begin(2000000);
  setupIO();
}

void setupIO() {
  pinMode(ENCODER_L_PHASE_A, INPUT);
  pinMode(ENCODER_L_PHASE_B, INPUT);
  pinMode(ENCODER_L_PHASE_A, INPUT_PULLUP);
  pinMode(ENCODER_L_PHASE_B, INPUT_PULLUP);

  pinMode(ENCODER_R_PHASE_A, INPUT);
  pinMode(ENCODER_R_PHASE_B, INPUT);
  pinMode(ENCODER_R_PHASE_A, INPUT_PULLUP);
  pinMode(ENCODER_R_PHASE_B, INPUT_PULLUP);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
}

// String lengths to transmit
int logLen = 0;
int rLengths[1000];
int lLengths[1000];

// String length tracking
#define LOG_STEP 2
int rLastLog = 0;
int lLastLog = 0;

// Encoder reading
int countL = 550; 
int prevL = HIGH;
int countR = 550; 
int prevR = HIGH;

// Button reading
bool recording = false;
int buttonPrev = HIGH;
bool timing = false;
unsigned long debounceTimer;

void loop() {
  checkEncoders();
  checkButton();
}

void checkEncoders() {
  int l = digitalRead(ENCODER_L_PHASE_A);
  if (l != prevL && l == HIGH){ // Pulse occurred
    if (digitalRead(ENCODER_L_PHASE_B) != l) {
      countL++;
    } else {
      countL--;
    }
    trackStrings();
  }
  prevL = l;
  
  int r = digitalRead(ENCODER_R_PHASE_A);
  if (r != prevR && r == HIGH){ // Pulse occurred
    if (digitalRead(ENCODER_R_PHASE_B) != r) {
      countR--;
    } else {
      countR++;
    }
    trackStrings();
  }
  prevR = r;
}

void trackStrings() {
  if (recording) {
    rLastLog = countR;
    lLastLog = countL;
    rLengths[logLen] = countR;
    lLengths[logLen] = countL;
    logLen++;
    Serial.print(countL);
    Serial.print(", ");
    Serial.println(countR);
    
    if (logLen >= 1000) {
      logLen = 0;
    }
  }
}

void checkButton() {
  if (!timing) {
    if (digitalRead(BUTTON_PIN) != buttonPrev) {
      // Start timing
      debounceTimer = millis();
      timing = true;
    }
  } else if ((millis() - debounceTimer) > DEBOUNCE_DELAY) {
    // Time elapsed
    int reading = digitalRead(BUTTON_PIN);
    if (reading != buttonPrev) {
      recording = !reading;
      if (reading) {
        buttonUp();
      } else {
        buttonDown();
      }
      buttonPrev = reading;
      timing = false;
    }    
  }
}

void buttonDown() {
  Serial.println("START");
  digitalWrite(15, HIGH);
}

void buttonUp() {
  Serial.println("END");
  digitalWrite(15, LOW);
}
