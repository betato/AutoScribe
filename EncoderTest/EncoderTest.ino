#define A_PIN 4
#define B_PIN 5

#define BUTTON_PIN 16

void setup() {
  Serial.begin(2000000);
  Serial.println("Basic Encoder Test:");

  pinMode(A_PIN, INPUT);
  pinMode(B_PIN, INPUT);
  pinMode(A_PIN, INPUT_PULLUP);
  pinMode(B_PIN, INPUT_PULLUP);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  //attachInterrupt(digitalPinToInterrupt(A_PIN), lowA, FALLING);
}

int count = 0; 
bool a;
bool aPrev;

bool button;
bool buttonPrev;

void loop() { 
  a = digitalRead(A_PIN);
  if (a != aPrev){ // Pulse occurred
    if (digitalRead(B_PIN) != a) {
      // Clockwise
      count++;
    } else {
      // Counter-clockwise
      count--;
    }
    Serial.println(count/1200.0, 4);
  }
  aPrev = a;

  button = digitalRead(BUTTON_PIN);
  if (button != buttonPrev){ // Button pressed
    if (button) {
      buttonDown();
    } else {
      buttonUp();
    }
  }
  buttonPrev = button;
}

void buttonDown() {
  Serial.println("Button Pressed");
}

void buttonUp() {
  Serial.println("Button Released");
}

