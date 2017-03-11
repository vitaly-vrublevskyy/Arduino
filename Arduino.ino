#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> //Temperature
#include <Servo.h>
#include <Keypad.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

const int TOTAL_DAYS = 18;

const int TONE_PIN = 4; 
const int POWER_SLOT = 3;
const int TEMPERATURE_SLOT = 2;
const int SERVO_SLOT = 9;

int angle = 0;    // variable to store the servo position

float  temperature; // celsius
float deltaT = 0;

// LCD
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Temperature sensor
OneWire  ds(TEMPERATURE_SLOT);  // on pin 10 (a 4.7K resistor is necessary)

Servo servo;  // create servo object to control a servo

// Specail symbols
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
byte deltaChar[8] = {0b00000,0b00000,0b00000,0b00000,0b00100,0b01010,0b11111,0b00000};
int celsium_t = 15 + 16*13;

int currentDay = 1; //TODO:

unsigned long duration; //duration from the beginig 

boolean prevPowerMode;

// Keyboard
const byte ROWS = 4; // 4 строки
const byte COLS = 4; // 4 столбца
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11,10, 9, 8}; // 5,6, 7,8
byte colPins[COLS] = {7, 6, 5, 4}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/*
 * CRITICAL RESTRICTION:min/max 36-38 

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
  Serial.begin(9600); //Debug
  
  pinMode(POWER_SLOT, OUTPUT);
  
  pinMode(TONE_PIN, OUTPUT);
  
  initDisplay();

  tone(4, 196);
}


void loop()
{ 
  temperature = getTemperature();
  
  duration = int(millis() / 1000);

  handleKeyPress();

  printTemperature();

  validateTemparature();

  printDay();

  heartbeat();

  //playShortBeep();

  // Rotate each 2 hours
  const int fourHours = 4 * 60; // * 60;
  if(duration % fourHours == 0) { 
    shakeLeftCard();
  } else if (duration % fourHours == fourHours / 2) { 
    shakeRightCard();
  }
}

// Generates a pulse of duty cycle 50%
// and period 800 milliseconds
void playShortBeep()
{
  tone (TONE_PIN, 500); //включаем на 500 Гц
  delay(500); //ждем 100 Мс
  tone(TONE_PIN, 1000); //включаем на 1000 Гц
  delay(500); //ждем 100 Мс
  //digitalWrite(TONE_PIN, HIGH);
  //delay(400);
  //digitalWrite(TONE_PIN, LOW);
  //delay(400);
}

//------- Private methods -------

void validateTemparature() {
  // depends on day
//  float delta = 0.55;  
//  float expected = 37.15;
//
//  boolean inRange =  abs(temperature - expected) <  delta;

  Serial.println(duration);
  if (duration % 50 == 0) {
    digitalWrite(POWER_SLOT, HIGH);
  } else {
    digitalWrite(POWER_SLOT, LOW);
  }
}

void shakeLeftCard() {
  servo.attach(SERVO_SLOT);
  for (angle = 0; angle <= 180; angle += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  servo.detach();
}

void shakeRightCard() {
  servo.attach(SERVO_SLOT);
  for (angle = 180; angle >= 0; angle -= 1) { // goes from 180 degrees to 0 degrees
    servo.write(angle);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
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
  
  lcd.setCursor(11, 0);
  lcd.print("--/");
  lcd.print(TOTAL_DAYS);
  lcd.setCursor(0, 1);
  lcd.printByte(2);
  
}

void printDay() {
  // TODO:  if (day() != currentDay) { currentDay = day()}
  
  lcd.setCursor(11, 0);
  if (currentDay < 10){
    lcd.print("0"); // inject '0'
  }
  lcd.print(currentDay);
  if (currentDay > TOTAL_DAYS) {
    //TODO: tone + highlight 
  }

  lcd.setCursor(7, 1);
  lcd.print(getMinTemperature(true));
  lcd.setCursor(11, 1);
  lcd.print("-");
  lcd.print(getMaxTemperature(true));
  
}

void printDuration() {
  lcd.setCursor(11, 1);
  lcd.print(duration);
}

void heartbeat() {
  lcd.setCursor(9, 0);
  if (duration % 3 == 0){
    lcd.print(" ");
  } else {
    lcd.printByte(0);
  }
  
}

void highlightLCD() {
  if (currentDay % 2 == 0){
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
        
        Serial.print("  Temperature = ");
        Serial.print(temperature);
        Serial.println(" Celsius, ");
    }
}

float getTemperature(){
  byte i;
  byte present = 0;
  byte type_s = 0;   // the first ROM byte indicates which chip
  byte data[12];
  byte addr[8];
  float celsius;

  // FIXME: this annoing shit
    if ( !ds.search(addr)) {
      Serial.println("No more addresses.");
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

float getMinTemperature(boolean norm){
  float result;
  if (currentDay <= 12) {
    result = 36.6;
  } else if (currentDay > 13 && currentDay <= 15 ) {
    result = 37.3;
  } else {
    result = 37.2;
  }
  // apply delta
  if (!norm){
    result += deltaT;
  }
  return result;
}

float getMaxTemperature(boolean norm){
  float result;
  if (currentDay <= 12) {
    result = 37.7;
  } else if (currentDay > 13 && currentDay <= 15 ) {
    result = 37.5;
  } else {
    result = 37.2;
  }
  // apply delta
  if (!norm){
    result += deltaT;
  }
  return result;
}



void handleKeyPress() {
  char key = keypad.getKey();
  if (key) {
    //TODO: Process char
    // increse delta by 0.1
  }
}

