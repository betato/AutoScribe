#pragma once

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClient.h>

const char *ssid = "DeliciousSuspicious";
const char *pass =  "chamfers";
#define SERVER_ADDRESS "http://192.168.137.8"

#define ENCODER_L_PHASE_A 4
#define ENCODER_L_PHASE_B 5

#define ENCODER_R_PHASE_A 12
#define ENCODER_R_PHASE_B 14

#define BUTTON_PIN 16
#define DEBOUNCE_DELAY 50

void setup() {
  Serial.begin(2000000);
  setupIO();
  setupWifi();
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
}

WiFiClient wifiClient;
HTTPClient http;

void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  http.setTimeout(1000);
}

// String lengths to transmit
int logLen = 0;
short rLengths[1000];
short lLengths[1000];

// String length tracking
#define LOG_STEP 4
short rLastLog = 0;
short lLastLog = 0;

// Encoder reading
short countL = 0;
short countR = 0; 
int prevL = HIGH;
int prevR = HIGH;

// Button reading
int buttonPrev = HIGH;
bool timing = false;
unsigned long debounceTimer;
bool recording = false;

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
  }
  prevL = l;
  
  int r = digitalRead(ENCODER_R_PHASE_A);
  if (r != prevR && r == HIGH){ // Pulse occurred
    if (digitalRead(ENCODER_R_PHASE_B) != r) {
      countR++;
    } else {
      countR--;
    }
  }
  prevR = r;

  if (recording) {
    trackStrings();
  }
}

void trackStrings() {
  // Log any large movements
  if (abs(countR-rLastLog) > LOG_STEP || abs(countL-lLastLog) > LOG_STEP) {
    rLastLog = countR;
    lLastLog = countL;
    rLengths[logLen] = countR;
    lLengths[logLen] = countL;
    logLen++;
    Serial.print(countR);
    Serial.print(" ");
    Serial.println(countL);
    
    if (logLen >= 1000) {
      transmitLengths();
    }
  }
}

void transmitLengths() {
  Serial.println("\nTransmit Begin");
  http.begin(SERVER_ADDRESS);
  // Put both short arrays into a byte array alternating from left to right
  char b[logLen*4];
  for (int i=0; i<logLen; i++) {
    b[i*4] = (lLengths[i])&255;
    b[i*4+1] = (lLengths[i] << 8)&255;
    b[i*4+2] = (rLengths[i])&255;
    b[i*4+3] = (rLengths[i] << 8)&255;
    
    Serial.print("L:");
    Serial.print((lLengths[i])&255);
    Serial.print("-");
    Serial.print((lLengths[i] << 8)&255);
    Serial.print("\nR:");
    Serial.print((rLengths[i])&255);
    Serial.print("-");
    Serial.print((rLengths[i] << 8)&255);
    Serial.println("");
  }
  Serial.println(logLen*4);
  // Send it
  http.POST(b);
  logLen = 0;
  http.end();
  Serial.println("Transmit Complete");
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
      // Button pressed/released
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
  Serial.println("Button Pressed");
}

void buttonUp() {
  Serial.println("Button Released");
  transmitLengths();
}
