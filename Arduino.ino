#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> //Temperature

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

const int TOTAL_DAYS = 18;
const int POWER_SLOT = 3;
const int TEMPERATURE_SLOT = 2;

float  temperature; // celsius

// LCD
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Temperature sensor
OneWire  ds(TEMPERATURE_SLOT);  // on pin 10 (a 4.7K resistor is necessary)

// Specail symbols
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
int celsium_t = 15 + 16*13;

int day = 1; //TODO:

unsigned long duration; //duration from the beginig 

boolean prevPowerMode;

/*
 * CRITICAL RESTRICTION:min/max 37-38 

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
  Serial.begin(9600); //Temperature
  
  // initialize digital pin 13 as an output.
  pinMode(POWER_SLOT, OUTPUT);
  
  initDisplay();
}


void loop()
{
  temperature = getTemperature();
  
  duration = int(millis() / 1000);

  printTemperature();

  validateTemparature();
   
  showDay();

  heartbeat();

  //delay(1000);
}

//------- Private methods -------
/*
1-12 day 36.6-37.7
13-15 day 37.3-37.5
із 16 day 37.2
*/
void validateTemparature() {
  // depends on day
//  float delta = 0.55;  
//  float expected = 37.15;
//
//  boolean inRange =  abs(temperature - expected) <  delta;

  if (duration % 60 == 0) {
    digitalWrite(POWER_SLOT, HIGH);
  } else {
    digitalWrite(POWER_SLOT, HIGH);
  }
}


void initDisplay()
{
  lcd.init();                      
  lcd.backlight();
  lcd.createChar(0, clock);
  lcd.createChar(1, heart);
  lcd.home();
  lcd.print("--.--C");
  lcd.printByte(celsium_t);
  
  lcd.setCursor(11, 0);
  lcd.print(day);
  lcd.print(" day");
}

void showDay() {
  // TODO: detect day 
  lcd.setCursor(11, 0);
  lcd.print(day);
  if (day > TOTAL_DAYS) {
    //TODO: tone + highlight 
  }
}

void printDuration() {
  lcd.setCursor(11, 1);
  lcd.print(duration);
}

void heartbeat() {
  lcd.setCursor(8, 0);
  if (duration % 3 == 0){
    lcd.print(" ");
  } else {
    lcd.printByte(0);
  }
  
}

void highlightLCD() {
  if (day % 2 == 0){
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

