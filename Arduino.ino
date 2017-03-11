#include <iarduino_RTC.h>
#include <iarduino_RTC_DS3231.h>
#include <memorysaver.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> //Temperature
#include <Servo.h>



#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

const unsigned int TOTAL_DAYS = 18;

const int TONE_PIN = 4;
const int POWER_SLOT = 5; // 3
const int VENTILATION_SLOT = 3; //5
const int TEMPERATURE_SLOT = 2;
const int SERVO_SLOT = 9;

int angle = 0;    // variable to store the servo position

float  temperature; // celsius
float deltaT = 0;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Temperature sensor
OneWire  ds(TEMPERATURE_SLOT);  // on pin 10 (a 4.7K resistor is necessary)

Servo servo;  // create servo object to control a servo

// Specail symbols
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
byte deltaChar[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01010, 0b11111, 0b00000};
int celsium_t = 15 + 16 * 13;

int currentDay;

unsigned long duration; //duration from the beginig

boolean ventilation;

// TIME
iarduino_RTC time(RTC_DS3231);

/*
   CRITICAL RESTRICTION:min/max 36-38

  1-12 day 36.6-37.7
  13-15 day 37.3-37.5
  із 16 day 37.2

  Охолодження constrain(sensVal, 2, 20);
  2-4 день пять разів на день охолодження 2-3 хв
  До 15 суток збільшити охолодження до 20 хв
  із 16 37.2 (без перевертання)

  Поворот кожні дві год
*/


void setup()
{
  delay(300);

  Serial.begin(9600); //Debug

  pinMode(POWER_SLOT, OUTPUT);

  pinMode(VENTILATION_SLOT, OUTPUT);

  pinMode(TONE_PIN, OUTPUT);

  //tone(TONE_PIN, 196);

  time.begin();

  resetTime(); //FIXME:

  initDisplay();

 
}


void loop()
{
  int t = getTemperature();
  if (t != -1) {
    temperature = t;
  }

  duration = int(millis() / 1000);

  handleKeyPress();

  validateTemparature();

  handleVentilation();

  updateDisplay();

  //alarm();

  handleRotate();

  //debug()
}


//------- Private methods -------

void debug() {
  lcd.setCursor(0, 1);
  lcd.print(time.gettime("d-m, H:i:s"));
}



void handleRotate() {
  const int twoHours = 2 * 60 * 60;
  // Rotate each 1 hours
  if (duration % twoHours == 0) {
    shakeLeftCard();
  } else if (duration % twoHours == twoHours / 2) {
    shakeRightCard();
  }
}



//Random
void resetTime() {
  // only for testing
  randomSeed(analogRead(0));
  int randDay = int(random(16)) + 1;


  // сек  0 до 59,  мин,  час, день 1 до 31, месяц, год, день недели
  time.settime(1, 0, 0, randDay, 0, 0, 0);
}

// Generates a pulse of duty cycle 50%
// and period 800 milliseconds
void alarm()
{
  tone (TONE_PIN, 500); //включаем на 500 Гц
  delay(500); //ждем 100 Мс
  tone(TONE_PIN, 1000); //включаем на 1000 Гц
  delay(500); //ждем 100 Мс
}


void handleVentilation() {
  if (isVentilation()) {
    showRemainigTime(); //TODO: clear on the last few sec
    digitalWrite(VENTILATION_SLOT, HIGH);
  } else {
    digitalWrite(VENTILATION_SLOT, LOW);
  }
}

void showRemainigTime() {
  boolean isLastVentilationSeconds = time.minutes == getVentilationLength() && time.seconds > 58;
  if (isLastVentilationSeconds) {
    lcd.setCursor(0, 1);
    lcd.print("      ");
  } else {
    lcd.setCursor(0, 1);
    lcd.printByte(0);
    lcd.print(time.gettime("i:s"));
  }
}

/*
  Охолодження constrain(sensVal, 2, 20);
  2-4 день пять разів на день охолодження 2-3 хв
  До 15 суток збільшити охолодження до 20 хв
  із 16 37.2 (без перевертання)
*/

boolean isVentilation() {
  return time.day >= 2 && time.Hours % 4 == 0 && time.minutes <= getVentilationLength();
}

int getVentilationLength() {
  int day = time.day;
  int minutes = 0;
  if (day >= 2 && day <= 4 ) {
    minutes = 3;
  } else if (day > 4) {
    minutes = int(constrain(day * 1.333, 3, 20)) + 1;
  }
  return minutes;
}


void validateTemparature() {
  int tMin = 22; //FIXME: getMinTemperature(true);
  int tMax = getMaxTemperature(true);

  if (isVentilation()) {
    digitalWrite(POWER_SLOT, LOW);
  } else if (temperature < tMin) {
    digitalWrite(POWER_SLOT, HIGH);
  } else if (temperature >= tMax) {
    digitalWrite(POWER_SLOT, LOW);
  }
}

void shakeLeftCard() {
  servo.attach(SERVO_SLOT);
  for (angle = 0; angle <= 180; angle += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 100ms for the servo to reach the position
  }
  servo.detach();
}

void shakeRightCard() {
  servo.attach(SERVO_SLOT);
  for (angle = 180; angle >= 0; angle -= 1) { // goes from 180 degrees to 0 degrees
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 100ms for the servo to reach the position
  }
  servo.detach();
}

void initDisplay()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, clock);
  lcd.createChar(1, heart);
  lcd.createChar(2, deltaChar);
  lcd.home();
  lcd.print("--.--C");
  lcd.printByte(celsium_t);

  lcd.setCursor(12, 0);
  lcd.print("-/");
  lcd.print(TOTAL_DAYS);
  lcd.setCursor(0, 1);
  lcd.printByte(2); //delta symbol

}

void updateDisplay() {
  printDay();

  heartbeat();

  printTemperature();
}

void printDay() {
  if (time.day == currentDay) {
    return;
  }

  currentDay = time.day;
  if (time.day < 10) {
    lcd.setCursor(12, 0);
  } else {
    lcd.setCursor(11, 0);
  }
  lcd.print(currentDay);
  if (currentDay > TOTAL_DAYS) {
    //TODO: tone + highlight
  }

  lcd.setCursor(7, 1);
  lcd.print(getMinTemperature(false));
  lcd.setCursor(11, 1);
  lcd.print("-");
  lcd.print(getMaxTemperature(false));
}


void printDuration() {
  lcd.setCursor(11, 1);
  lcd.print(duration);
}

void heartbeat() {
  lcd.setCursor(9, 0);
  if (duration % 3 == 0) {
    lcd.print(" ");
  } else {
    lcd.printByte(1);
  }

}

void highlightLCD() {
  if (time.day % 2 == 0) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}

// Temperature
void printTemperature() {
  if (temperature != -1) {
    lcd.setCursor(0, 0);
    lcd.print(temperature);

    //Serial.print("  Temperature = ");
    //Serial.print(temperature);
    //Serial.println(" Celsius, ");
  }
}

float getTemperature() {
  byte i;
  byte present = 0;
  byte type_s = 0;   // the first ROM byte indicates which chip
  byte data[12];
  byte addr[8];
  float celsius;

  // FIXME: this annoing shit
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    ds.reset_search();
    delay(250);
    return -1;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }


  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  return celsius;
}




/*
  1-12 day 36.6-37.7
  13-15 day 37.3-37.5
  із 16 day 37.2
*/

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



void handleKeyPress() {
  /*
    char key = keypad.getKey();
    if (key) {
    resetTime();
    //TODO: Process char
    // increse delta by 0.1
    }
  */
}

