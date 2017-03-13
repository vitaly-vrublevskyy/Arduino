#include <iarduino_RTC.h>
#include <iarduino_RTC_DS3231.h>
#include <memorysaver.h>
#include <Wire.h>
#include <Servo.h>

const unsigned int TOTAL_DAYS = 18;
const unsigned int MAX_ANGLE = 45;

const int TONE_PIN = 4;
const int POWER_SLOT = 5; 
const int VENTILATION_SLOT = 3;
const int TEMPERATURE_SLOT = 6;
const int SERVO_SLOT = 9;


const int CRITICAL_HIGH_TEMPERATURE = 40;
const int CRITICAL_LOW_TEMPERATURE = 25;
float  temperature; // celsius
float deltaT = 0;

boolean ventilationFlag;
boolean heatingFlag;

unsigned long duration; //duration from the beginig Arduino


// Motor
Servo servo;  // create servo object to control a servo

// Clock
iarduino_RTC time(RTC_DS3231);


void setup()
{
  delay(300);

  Serial.begin(9600); //Debug

  pinMode(POWER_SLOT, OUTPUT);
  pinMode(VENTILATION_SLOT, OUTPUT);
  pinMode(TONE_PIN, OUTPUT);

  // PROD MODE: tone(TONE_PIN, 196);

  time.begin();
  resetTime(); //DEV MODE:

  initDisplay();

  servo.write(0); // prevent issue
}



void loop()
{
  temperature = getTemperature();
  
  if (temperature) {
    
      boolean ventilation = isVentilation();

      duration = int(millis() / 1000);
    
      handleKeyPress();
      
      handleVentilation(ventilation);
    
      validateTemparature(ventilation); // after handle ventilation
    
      handleRotate(); 
    
      updateDisplay(ventilation);
  }
}

//------- Private methods -------


/**
  *Rotate
  */

// Rotate each hour
void handleRotate() {
  const int someMinute = 3;
  boolean isZeroAngle = servo.read() < 5; // check angle : 93 - magic on init
  //TODO: time.Hours % 2 == 0 && time.minutes == someMinute
  
  if (time.minutes % 2 == 0 && isZeroAngle) {
    servo.attach(SERVO_SLOT);
    rotateLeft();
    servo.detach();
  //TODO: time.Hours % 2 == 1  time.minutes == someMinute
  } else if (time.minutes % 2 == 1 && !isZeroAngle) {
    servo.attach(SERVO_SLOT);
    rotateRight();
    servo.detach();
  } 
}

void rotateLeft() {
  int start = constrain(servo.read(), 0, MAX_ANGLE);
  Serial.print("  RotateLeft = ");
  Serial.println(start);
  for (int angle = start; angle <= MAX_ANGLE; angle += 1) { // goes from 0 degrees to MAX_ANGLE degrees
    // in steps of 1 degree
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 100ms for the servo to reach the position
  }
  delay(300); //pause
}

void rotateRight() {
  int start = constrain(servo.read(), 0, MAX_ANGLE);
  Serial.print("  RotateRight = ");
  Serial.println(start);
  for (int angle = start; angle >= 0; angle -= 1) { // goes from MAX_ANGLE degrees to 0 degrees
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 100ms for the servo to reach the position
  }
  delay(300); // pause 

  //FIXME: cause of issue
  Serial.print("  RotateRight (end) = ");
  Serial.println(servo.read());
}

void rotateToZero(){
  servo.attach(SERVO_SLOT);
  servo.write(0);
  servo.detach();
}


/** Temperature
  1-12 day 36.6-37.7
  13-15 day 37.3-37.5
  із 16 day 37.2
*/
void validateTemparature(boolean ventilation) {
  int tMin = getMinTemperature(true);
  int tMax = getMaxTemperature(true);

  if (ventilation) {
    heating(false);
  } else if (temperature < tMin) {
    // PROD: if (temperature < CRITICAL_LOW_TEMPERATURE) { alarm(); }
    heating(true);
  } else if (temperature >= tMax) {
    heating(false);
  } else if (temperature >= CRITICAL_HIGH_TEMPERATURE) {
    // PROD: alarm(); 
  }
}

void heating(boolean turnOn) {
  if (heatingFlag != turnOn) {
    heatingFlag = turnOn;
    if (turnOn) {
      digitalWrite(POWER_SLOT, HIGH);  
    } else {
      digitalWrite(POWER_SLOT, LOW);
    }
  }
}

float getMinTemperature(boolean applyDelta) {
  float result;
  int day =  time.day;
  if ( day <= 12) {
    result = 36.6;
  } else if (day >= 13 && day <= 15 ) {
    result = 37.3;
  } else {
    result = 37.2;
  }

  if (applyDelta) {
    result += deltaT;
  }
  return result;
}

float getMaxTemperature(boolean applyDelta) {
  float result;
  int day = time.day;
  if (day <= 12) {
    result = 37.7;
  } else if (day >= 13 && day <= 15 ) {
    result = 37.5;
  } else {
    result = 37.2;
  }

  if (applyDelta) {
    result += deltaT;
  }
  return result;
}



/**
 * Ventilation
  2-4 день пять разів на день охолодження 2-3 хв
  До 15 суток збільшити охолодження до 20 хв
  із 16 37.2 (без перевертання)
 */

void handleVentilation(boolean turnOn) {
  turnOn = turnOn || temperature >= CRITICAL_HIGH_TEMPERATURE;
  
  if (ventilationFlag != turnOn ){
    ventilationFlag = turnOn;
    if (turnOn) {
      digitalWrite(VENTILATION_SLOT, HIGH);
    } else {
      // TODO: try to single write on change
      digitalWrite(VENTILATION_SLOT, LOW);
    }
  }
}

boolean isVentilation() {
  return time.day >= 2 && 
         time.Hours % 4 == 0 && 
         time.minutes <= getVentilationLength();
}

int getVentilationLength() {
  //return 0; // FIXME: debugger
  
  int day = time.day;
  const float multiplier = 1.333;
  int minutes = 0;
  if (day >= 2 && day <= 4 ) {
    minutes = 3;
  } else if (day > 4) {
    minutes = int(constrain(day * multiplier, 3, 20)) + 1;
  }
  return minutes;
}


/**
 *  Key Press
 */
void handleKeyPress() {
  /*
    char key = keypad.getKey();
    if (key) {  //TODO: Process ket
    
    resetTime();

    rotateToZero();

    // increse delta by 0.1
    }
  */
}

/**
* Util
*/
void resetTime() {
  // only for testing
  randomSeed(analogRead(0));
  int randDay = int(random(16)) + 1;

  // сек  0 до 59,  мин,  час, день 1 до 31, месяц, год, день недели
  time.settime(0, 0, 0, randDay, 0, 0, 0);
}


void alarm() {
  tone (TONE_PIN, 500); //включаем на 500 Гц
}


