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
  //setupWifi();
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

#define SERVER_ADDRESS "http://192.168.137.70"
#define SERVER_PORT 9000

void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

// String lengths to transmit
int logLen = 0;
int rLengths[1000];
int lLengths[1000];

WiFiClient wifiClient;
HTTPClient http;

// String length tracking
#define LOG_STEP 2
int rLastLog = 0;
int lLastLog = 0;

// Encoder reading
int countL = 10; 
int prevL = HIGH;
int countR = 10; 
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
      countL--;
    } else {
      countL++;
    }
    trackStrings();
  }
  prevL = l;
  
  int r = digitalRead(ENCODER_R_PHASE_A);
  if (r != prevR && r == HIGH){ // Pulse occurred
    if (digitalRead(ENCODER_R_PHASE_B) != r) {
      countR++;
    } else {
      countR--;
    }
    trackStrings();
  }
  prevR = r;
}

void trackStrings() {
  if (recording) {
    // Log any large movements
    //if (abs(countR-rLastLog) > LOG_STEP || abs(countL-lLastLog) > LOG_STEP) {
      rLastLog = countR;
      lLastLog = countL;
      rLengths[logLen] = countR;
      lLengths[logLen] = countL;
      logLen++;
      Serial.print(countL);
      Serial.print(", ");
      Serial.println(countR);
      
      if (logLen >= 1000) {
        //transmitLengths();
        logLen = 0;
      }
    //}
  }
}

void transmitLengths() {
  Serial.println("aaaa");
  http.begin(SERVER_ADDRESS, SERVER_PORT);
  http.addHeader("Content-Type", "text/html");
  int httpCode = http.POST("aaa");
  http.end();
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
  Serial.println("Button Pressed");
  digitalWrite(15, HIGH);
  //transmitLengths();
}

void buttonUp() {
  Serial.println("Button Released");
  digitalWrite(15, LOW);
}
