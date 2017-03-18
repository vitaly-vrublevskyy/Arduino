#include <LiquidCrystal_I2C.h>


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif


const byte BLINK_DELAY = 4;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Specail symbols
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
byte deltaChar[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01010, 0b11111, 0b00000};
byte windChar[8] = {0b00000,0b11001,0b00101,0b01110,0b10100,0b10011,0b00000,0b00000};
byte heatingChar[8] = {0b00100,0b11010,0b01010,0b11010,0b01010,0b10001,0b10001,0b01110};
byte arrowUp[8] = {0b00111,0b00011,0b00101,0b01000,0b10000,0b00000,0b00000,0b00000};
byte quailChar[8] = {0b00000,0b00000,0b00000,0b00100,0b01110,0b01110,0b01110,0b00100};
byte chickenChar[8] = {0b00100,0b01110,0b11111,0b11111,0b11111,0b11111,0b01110,0b00000};
byte celsium_t = 15 + 16 * 13;

int currentDay;

void initDisplay()
{
  lcd.init();
  
  lcd.backlight();

  // Init specail character
  lcd.createChar(0, clock);
  lcd.createChar(1, heart);
  lcd.createChar(2, deltaChar);
  lcd.createChar(3, windChar);
  lcd.createChar(4, heatingChar);
  lcd.createChar(5, arrowUp);
  lcd.createChar(6, quailChar);
  lcd.createChar(7, chickenChar);
  
  lcd.home();
}


byte getTotalDays() {
    return CHICKEN_MODE ? 21 : 18;
}

void updateDisplay(float temperature, int humidity, boolean ventilation, boolean turnOnHeat) {

  printTemperature(temperature);

  printHumidity(humidity);

  // Handle status info  
  if (ventilation) {
    printRemainigVentilationTime();
  } else if (turnOnHeat) {
    blinkHeating(turnOnHeat);
  } else {
    clearPixels();
  }

  if (duration % 30 < 10) {
    printDay();
  } else if (duration % 30 < 20)  {
    printTemperatureRange();
  } else {
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.print(time.gettime("H:i:s"));
  }
}

void printDay() {
  lcd.setCursor(7, 1);
  lcd.print("     ");
  
  //heartbeat
  lcd.setCursor(10, 1);
  lcd.printByte(1);

  
  int day = time.day;
  if (day < 10) {
    lcd.setCursor(12, 1);
  } else {
    lcd.setCursor(11, 1);
  }
  
  lcd.print(day);
  lcd.print("/");
  lcd.print(getTotalDays());

  if (day >= getTotalDays() && time.Hours == 0 && time.minutes == 0) {
    highlightLCD();
    alarm();
  }
}

void printHumidity(int h){
  lcd.setCursor(0, 1);
  lcd.print(h);
  lcd.print("%");
}

void printTemperatureRange() {
    currentDay = time.day;
    lcd.setCursor(7, 1);
    lcd.print(getMinTemperature());
    lcd.setCursor(11, 1);
    lcd.print("-");
    lcd.print(getMaxTemperature());
}


void blinkHeating(boolean turnOnHeat) {
  lcd.setCursor(10, 0);
  
  if (duration % BLINK_DELAY == 0) {
    lcd.print("      ");
  } else {
    lcd.printByte(4);
    lcd.print("t");
    lcd.printByte(celsium_t);
    //lcd.printByte(5); //arrow up:
  }
}

void printTemperature(float temperature) {
  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.print("C");
  lcd.printByte(celsium_t);
}

void printRemainigVentilationTime() {
  lcd.setCursor(10, 0);
  if (duration % 3 == 0) {
    lcd.print(" ");
  } else {
    lcd.printByte(3);
  }
  lcd.print(time.gettime("i:s")); 
}

void clearPixels() {
   lcd.setCursor(10, 0);
   lcd.print("      ");
}

void highlightLCD() {
  if (duration % 4 < 2) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}
