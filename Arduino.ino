#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <Time.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
int celsium_t = 15 + 16*13;
int day = 1;
/*
 * CRITICAL RESTRICTION:min/max 37-38
1-12 day 36.6-37.7
13-15 day 37.3-37.5
із 16 day 37.2

Охолодження
2-4 день пять разів на день охолодження 2-3 хв
До 15 суток збільшити охолодження до 20 хв
*/


void setup()
{
  initDisplay();
}


void loop()
{ 
  showDay();
  
  delay(5000);
}

//------- Private methods -------

void initDisplay()
{
  lcd.init();                      
  //lcd.backlight();
  lcd.createChar(0, clock);
  lcd.home();
  lcd.print("17.45C");
  lcd.printByte(celsium_t);
  
  lcd.setCursor(11, 0);
  lcd.print(day);
  lcd.print(" day");
}

void showDay() {
  // TODO: detect day 
  lcd.setCursor(11, 0); 
  lcd.print(++day);
}

void highlightLCD() {
  if (day % 2 == 0){
    lcd.backlight(); 
  } else {
    lcd.noBacklight();
  }
}

