#include <Servo.h>

const byte MAX_ANGLE = 60;

// Motor
Servo servo;  // create servo object to control a servo


void initServo(){
  servo.write(0); // !prevent issue: magic value 93
}


/**
  * Rotate each hour in some certain minute. E.g. 7th minute
  */
void handleRotate() { 
  /*int minutes = duration / 60;
  int hours = duration / (60 * 60);*/
  const byte certianMinute = 7;
  if (time.Hours % 2 == 0 && time.minutes == certianMinute) {
    rotateLeft();
  } else if (time.Hours % 2 == 1 && time.minutes == certianMinute) {
    rotateRight();
  } 
}

void rotateLeft() {  
  byte startAngle = constrain(servo.read(), 0, MAX_ANGLE);
  boolean isStartLimitAngle = startAngle < 10;
  if (isStartLimitAngle) {
    Serial.print(time.gettime("d-m-Y, H:i:s,"));
    Serial.print("  RotateLeft = ");
    Serial.println(startAngle);

    servo.attach(SERVO_SLOT); 
    for (int angle = startAngle; angle <= MAX_ANGLE; angle += 1) { // goes from 0 degrees to MAX_ANGLE degrees
      servo.write(angle);              // tell servo to go to position in variable 'pos'
      delay(100);                      // waits 100ms for the servo to reach the position
    }
    delay(300); // pause
    servo.detach();
  }
}


void rotateRight() {
  byte startAngle = constrain(servo.read(), 0, MAX_ANGLE);
  boolean isStartLimitAngle = abs(startAngle - MAX_ANGLE) < 10;
  if (isStartLimitAngle) {
    Serial.print(time.gettime("d-m-Y, H:i:s,"));
    Serial.print(":  RotateRight = ");
    Serial.println(startAngle);

    servo.attach(SERVO_SLOT);
    for (int angle = startAngle; angle >= 0; angle -= 1) { // goes from MAX_ANGLE degrees to 0 degrees
      servo.write(angle);              // tell servo to go to position in variable 'pos'
      delay(100);                      // waits 100ms for the servo to reach the position
    }
    delay(300); // pause before detach
    servo.detach();
  }
}

void rotateToZero(){
  servo.attach(SERVO_SLOT);
  servo.write(0);
  servo.detach();
}


