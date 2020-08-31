#include <Keypad.h>
#include <Joystick.h>

const byte ROTARIES = 4;
const byte JOYSTICKS = 1;

const byte ROWS = 4;
const byte COLS = 3;

const byte CW = 1;
const byte CCW  = 2;
const byte STOP = 0;

class Rotary {
  public:
    byte pin1;
    byte pin2;
    byte ccwKey;
    byte cwKey;

    int previousStateCLK;
};

struct MyJoystick {
  byte pinX;
  byte pinY;

  int lowLimit;
  int highLimit;

  byte lowXKey;
  byte highXKey;
  byte lowYKey;
  byte highYKey;
};

Rotary rotaries[ROTARIES] {
  {0, 1, 19, 20, 0},
  {2, 3, 17, 18, 0},
  {4, 5, 15, 16, 0},
  {6, 7, 13, 14, 0},
};

MyJoystick joysticks[JOYSTICKS] {{A9, A8, 50, 970, 30, 31, 28, 29}};


byte buttons[ROWS][COLS] = {
  {8, 6, 4},
  {9, 7, 5},
  {10, 11, 0},
  {3, 2, 1},
};

byte rowPins[ROWS] = {21, 20, 19, 16};
byte colPins[COLS] = {18, 15, 14};

Keypad myKeypad = Keypad( makeKeymap(buttons), rowPins, colPins, ROWS, COLS);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 32, 0,
                   false, false, false, false, false, false,
                   false, true, false, false, false);

void setup() {
  Joystick.begin();
  setupRotaries();
  setupJoysticks();
  setupHandbrake();

}

void loop() {
  loopEncoders();
  loopButtons();
  loopJoysticks();
  loopHandbrake();

}

void setupRotaries() {
  for (Rotary& rot : rotaries) {
    pinMode(rot.pin1, INPUT);
    pinMode(rot.pin2, INPUT);
    pinMode(rot.pin1, INPUT_PULLUP);
    pinMode(rot.pin2, INPUT_PULLUP);
    digitalWrite(rot.pin1, HIGH);
    digitalWrite(rot.pin2, HIGH);

    rot.previousStateCLK = 0;
  }
}

void setupJoysticks() {

    for (MyJoystick joy : joysticks) {
      pinMode(joy.pinX, INPUT);
      pinMode(joy.pinY, INPUT);
    }
}

void setupHandbrake() {
  pinMode(A10, INPUT);
  Joystick.setThrottleRange(100, 950);
}

void loopHandbrake() {
  
  int pot = analogRead(A10);
  Joystick.setThrottle(pot);
}

void loopButtons() {
  if (myKeypad.getKeys()) {
    for (Key key : myKeypad.key) {
      if (key.stateChanged) {
        switch (key.kstate) {
          case PRESSED:
          case HOLD:
            Joystick.setButton(key.kchar, 1);
            break;
          case RELEASED:
          case IDLE:
            Joystick.setButton(key.kchar, 0);
            break;
        }
      }
    }
  }
}

void loopEncoders() {
  for (Rotary& rot : rotaries) {
    int result = getRotaryDir(&rot);
    if (result == CCW) {
      Joystick.setButton(rot.ccwKey, 1);
      delay(50);
      Joystick.setButton(rot.ccwKey, 0);
    }
    if (result == CW) {
      Joystick.setButton(rot.cwKey, 1);
      delay(50);
      Joystick.setButton(rot.cwKey, 0);
    }
  }
}
void loopJoysticks() {
  for(MyJoystick joy : joysticks) {
    loopJoystick(joy);
  }
}

void loopJoystick(MyJoystick myJoystick) {

  if (analogRead(myJoystick.pinX) < myJoystick.lowLimit) {
    Joystick.setButton(myJoystick.lowXKey, 1);
  } else {
    Joystick.setButton(myJoystick.lowXKey, 0);
  }

  if (analogRead(myJoystick.pinX) > myJoystick.highLimit) {
    Joystick.setButton(myJoystick.highXKey, 1);
  } else {
    Joystick.setButton(myJoystick.highXKey, 0);
  }


  if (analogRead(myJoystick.pinY) < myJoystick.lowLimit) {
    Joystick.setButton(myJoystick.lowYKey, 1);
  } else {
    Joystick.setButton(myJoystick.lowYKey, 0);
  }


  if (analogRead(myJoystick.pinY) > myJoystick.highLimit) {
    Joystick.setButton(myJoystick.highYKey, 1);
  } else {
    Joystick.setButton(myJoystick.highYKey, 0);
  }
}



byte getRotaryDir(Rotary *rotary) {
  int currentStateCLK = digitalRead(rotary->pin1);
  int encdir = STOP;


  if (currentStateCLK != rotary->previousStateCLK) {
    if (digitalRead(rotary->pin2) != currentStateCLK) {
      encdir = CCW;
    } else {
      encdir = CW;
    }
    rotary->previousStateCLK = currentStateCLK;
  }
  return encdir;
}
