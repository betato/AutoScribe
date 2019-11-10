#define ENCODER_L_PHASE_A 4
#define ENCODER_L_PHASE_B 5

#define ENCODER_R_PHASE_A 12
#define ENCODER_R_PHASE_B 14

#define BUTTON_PIN 16
#define DEBOUNCE_DELAY 50

void setup() {
  Serial.begin(2000000);

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

int countL = 0; 
int prevL = HIGH;

int countR = 0; 
int prevR = HIGH;

int buttonPrev = HIGH;
bool timing = false;
unsigned long debounceTimer;

void loop() {
  checkEncoders();
  checkButton();
}

void checkEncoders() {
  int l = digitalRead(ENCODER_L_PHASE_A);
  if (l != prevL){ // Pulse occurred
    if (digitalRead(ENCODER_L_PHASE_B) != l) {
      countL++;
    } else {
      countL--;
    }
    Serial.print("L:");
    Serial.println(countL/1200.0, 4);
  }
  prevL = l;
  
  int r = digitalRead(ENCODER_R_PHASE_A);
  if (r != prevR){ // Pulse occurred
    if (digitalRead(ENCODER_R_PHASE_B) != r) {
      countR++;
    } else {
      countR--;
    }
    Serial.print("R:");
    Serial.println(countR/1200.0, 4);
  }
  prevR = r;
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
}

