#include "Mouse.h"

const int STICK_POWER = 10;
const int X_AXIS_PIN = 8;
const int Y_AXIS_PIN = 9;
const int TOGGLE_PIN = 7;

const int LEFT_BUTTON_LED_PIN = 4;
const int RIGHT_BUTTON_LED_PIN = 5;
const int LEFT_BUTTON_PIN = 2;
const int RIGHT_BUTTON_PIN = 3;

const int DELAY_MILLI_SECONDS = 20;
const int MAX_MOVE = 30;
const int NATURAL_MERGIN = 50;

int toggle = 1;

int xNatural = 0;
int yNatural = 0;

int xMax = 0;
int xMin = 0;
int yMax = 0;
int yMin = 0;

int yVal = 0;
int xVal = 0;
int xManipulated = 0;
int yManipulated = 0;

int leftButton = 1;
int rightButton = 1;

int prevLeftButton = 1;
int prevRightButton = 1;

void setup() {
  Serial.begin(9600);

  pinMode(TOGGLE_PIN, INPUT_PULLUP);
  pinMode(STICK_POWER, OUTPUT);
  pinMode(LEFT_BUTTON_LED_PIN, OUTPUT);
  pinMode(RIGHT_BUTTON_LED_PIN, OUTPUT);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(STICK_POWER, HIGH);
  digitalWrite(LEFT_BUTTON_LED_PIN, HIGH);
  digitalWrite(RIGHT_BUTTON_LED_PIN, HIGH);
  initValues();
}

void loop() {
  readToggle();
  readButton();
  readStick();
  
  updateLEDs();
  buttonEvent();
  cursorMove();
  printDebug();
  delay(DELAY_MILLI_SECONDS); 
}

void cursorMove() {
  if (toggle == 0) {
    xManipulated = calcManipulatedValue(xVal, xNatural, xMax, xMin);
    yManipulated = calcManipulatedValue(yVal, yNatural, yMax, yMin);
 
    if (xManipulated != 0 || yManipulated != 0) {
      Mouse.move(xManipulated, yManipulated, 0);
    }
  }
}

int calcManipulatedValue(int val, int natural, int max_val, int min_val) {
  int result = 0;

  if (val > (natural + NATURAL_MERGIN)) {
    int manipulatedZeroPosition = natural + NATURAL_MERGIN;
    float ratio = ((float)(val - manipulatedZeroPosition)) / (max_val - manipulatedZeroPosition);
    result = ratio * ratio * MAX_MOVE;
  } else if (val < (natural - NATURAL_MERGIN)) {
    int manipulatedZeroPosition = natural - NATURAL_MERGIN;
    float ratio = ((float)(manipulatedZeroPosition- val)) / (manipulatedZeroPosition - min_val);
    result = ratio * ratio * -MAX_MOVE;
  }

  return result;
}

void updateMouseMode() {
  if (toggle == 0) {
    Mouse.begin();
  } else {
    Mouse.end();
  }
}

void updateLEDs() {
  if (toggle == 0) {
    digitalWrite(LEFT_BUTTON_LED_PIN, HIGH);
    digitalWrite(RIGHT_BUTTON_LED_PIN, HIGH);
  } else {
    digitalWrite(LEFT_BUTTON_LED_PIN, LOW);
    digitalWrite(RIGHT_BUTTON_LED_PIN, LOW);
  }
}

void readToggle() {
  toggle = digitalRead(TOGGLE_PIN);
}

void readButton() {
  leftButton = digitalRead(LEFT_BUTTON_PIN);
  rightButton = digitalRead(RIGHT_BUTTON_PIN);
}

void buttonEvent() {
  if (toggle == 0) {
    if (leftButton != prevLeftButton) {
      if (leftButton == 0) {
        Mouse.press(MOUSE_LEFT);
      } else {
        Mouse.release(MOUSE_LEFT);
      }
    }
    
    if (rightButton != prevRightButton) {
      if (rightButton == 0) {
        Mouse.press(MOUSE_RIGHT);
      } else {
        Mouse.release(MOUSE_RIGHT);
      }
    }

    prevLeftButton = leftButton;
    prevRightButton = rightButton;
  }
}

void readStick() {
  yVal = analogRead(Y_AXIS_PIN);
  xVal = analogRead(X_AXIS_PIN);

  if (xVal > xMax) {
    xMax = xVal;
  } else if (xVal < xMin) {
    xMin = xVal;
  }
           b   
  if (yVal > yMax) {
    yMax = yVal;
  } else if (yVal < yMin) {
    yMin = yVal;
  }
}

void initValues() {
  readStick();
  xNatural = xVal;
  yNatural = yVal;
   
  xMax = xNatural + 100;
  xMin = xNatural - 100;
  yMax = yNatural + 100;
  yMin = yNatural - 100;
}

void printDebug() {
  Serial.print("toggle, left, right, xVal, yVal, xManipulated, yManipulated = ");
  Serial.print(toggle);
  Serial.print(", ");
  Serial.print(leftButton);
  Serial.print(", ");
  Serial.print(rightButton);
  Serial.print(", ");
  Serial.print(xVal);
  Serial.print(", ");
  Serial.print(yVal);
  Serial.print(", ");
  Serial.print(xManipulated);
  Serial.print(",");
  Serial.println(yManipulated);
}
