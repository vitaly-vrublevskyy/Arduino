#include <iarduino_RTC.h>
#include <iarduino_RTC_DS3231.h>
#include <memorysaver.h>
#include <Wire.h>

const int TONE_PIN = 4;
const int POWER_SLOT = 5; 
const int VENTILATION_SLOT = 3;
const int TEMPERATURE_SLOT = 6;
const int SERVO_SLOT = 9;

unsigned long duration; //duration from the beginig Arduino


// Clock
iarduino_RTC time(RTC_DS3231);


void setup()
{
  Serial.begin(9600); //Debug
  
  delay(300);

  pinMode(POWER_SLOT, OUTPUT);
  pinMode(VENTILATION_SLOT, OUTPUT);
  pinMode(TONE_PIN, OUTPUT);

  time.begin();
  
  //resetTime(); //DEV MODE:

  initDisplay();

  initServo(); // !prevent issue

  // PROD MODE: tone(TONE_PIN, 196);
}


void loop()
{
  handleKeyPress();
  
  duration = int(millis() / 1000);

  time.gettime(); // force update
  
  float temperature = getTemperature(); // Temperature.h
  
  if (temperature) {
    
      boolean ventilation = isVentilation(); // Ventilation.h
      
      handleVentilation(temperature, ventilation); // Ventilation.h
    
      validateTemparature(temperature, ventilation); // Heating.h

      if (duration > 20 && time.day < 16) {
         handleRotate(); // Rotate.h 
      }
    
      updateDisplay(temperature, ventilation); //  // Display.h
  }
}



/**
* Util
*/
void resetTime() {
  // only for testing
  randomSeed(analogRead(0));
  int randDay = int(random(14)) + 1;

  // сек  0 до 59,  мин,  час, день 1 до 31, месяц, год, день недели
  time.settime(0, 0, 0, randDay, 0, 0, 0);
}


void alarm() {
  tone (TONE_PIN, 500); //включаем на 500 Гц
}




/**
 *  Key Press : TTP229
 */
void handleKeyPress() {
/*  char key = ...
    if (key) {  //TODO: Process ket
    resetTime();
    rotateToZero();
    // increse delta by 0.1
    } */
}

