#include <iarduino_RTC.h>
#include <iarduino_RTC_DS3231.h>
#include <memorysaver.h>
#include <Wire.h>

const boolean PROD = false;

const boolean CHICKEN_MODE = false;

const int TONE_PIN = 4;
const int POWER_SLOT = 5; 
const int VENTILATION_SLOT = 3;
const int TEMPERATURE_SLOT = 6;
const int DHT_SLOT = 8;
const int SERVO_SLOT = 9;


// Clock
iarduino_RTC time(RTC_DS3231);

unsigned long duration; //duration from the beginig Arduino


void setup()
{
  Serial.begin(9600); //Debug
  
  delay(300);

  randomSeed(analogRead(0));

  pinMode(POWER_SLOT, OUTPUT);
  pinMode(VENTILATION_SLOT, OUTPUT);
  pinMode(TONE_PIN, OUTPUT);

  time.begin();
  
  //resetTime(); //DEV MODE:

  initDisplay();

  initServo(); // !prevent issue

  if (PROD)
    tone(TONE_PIN, 196);
}


void loop()
{
  handleKeyPress();
  
  duration = int(millis() / 1000);

  time.gettime(); // force update
  
  float temperature = getTemperature(); // Temperature.h
  int humidity = getHumidity();
  
  if (temperature && humidity) {
    
      boolean ventilation = isVentilation(); // Ventilation.h
      
      handleVentilation(temperature, ventilation); // Ventilation.h
    
      boolean turnOnHeat = validateTemparature(temperature, ventilation); // Heating.h

      handleRotate(); // Rotate.h

      updateDisplay(temperature, humidity, ventilation, turnOnHeat);  // Display.h
  }
}



/**
* Util
*/
void resetTime() {
  
  int randDay = int(random(14)) + 1;

  // сек  0 до 59,  мин,  час, день 1 до 31, месяц, год, день недели
  time.settime(0, 0, 0, randDay, 0, 0, 0);
}


void alarm() {
  if (duration % 6 < 3)
    tone (TONE_PIN, 500); //включаем на 500 Гц
  else
    tone (TONE_PIN, 1000);
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

