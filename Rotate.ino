#include <Servo.h>

const unsigned int MAX_ANGLE = 45;

// Motor
Servo servo;  // create servo object to control a servo


void initServo(){
  servo.write(0); // !prevent issue: magic value 93
}


/**
  * Rotate each hour in some certain minute. E.g. 7th minute
  */

void handleRotate() {
  //const int someMinute = 7;
  //TODO: time.Hours % 2 == 0 && time.minutes == someMinute
  if (time.minutes % 2 == 0 && duration > 30) {
    rotateLeft();  
  //TODO: time.Hours % 2 == 1  time.minutes == someMinute
  } else if (time.minutes % 2 == 1 && duration > 30) {
    rotateRight();
  } 
}

void rotateLeft() {  
  int startAngle = constrain(servo.read(), 0, MAX_ANGLE);
  boolean isStartLimitAngle = startAngle < 10;
  if (isStartLimitAngle) {
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


/* // no lock
void rotateLeftV2() {
  if (servo.read() < MAX_ANGLE){
    servo.attach(SERVO_SLOT); 
    const int delta = 3;
    int angle = constrain(servo.read() + delta, 0, MAX_ANGLE);
    servo.write(angle); 
  } else {
    servo.detach(); 
  }
}
*/

void rotateRight() {
  int startAngle = constrain(servo.read(), 0, MAX_ANGLE);
  boolean isStartLimitAngle = abs(startAngle - MAX_ANGLE) < 10;
  if (isStartLimitAngle) {
    Serial.print("  RotateRight = ");
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


