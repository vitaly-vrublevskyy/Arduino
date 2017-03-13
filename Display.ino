#include <LiquidCrystal_I2C.h>


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif


// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Specail symbols
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
byte deltaChar[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01010, 0b11111, 0b00000};
byte windChar[8] = {0b00000,0b11001,0b00101,0b01110,0b10100,0b10011,0b00000,0b00000};
int celsium_t = 15 + 16 * 13;

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
  
  lcd.home();
  
  lcd.print("--.--C");
  lcd.printByte(celsium_t);

  lcd.setCursor(12, 0);
  lcd.print("-/");
  lcd.print(TOTAL_DAYS);
  lcd.setCursor(0, 1);
  lcd.printByte(2); //delta symbol

}

void highlightLCD() {
  unsigned long seconds = int(millis() / 1000);
  if (seconds % 10 > 5) { // 
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}


void updateDisplay(boolean ventilation) {
  printTemperature();
  
  printDay();

  heartbeat();

  if (ventilation) {
    printRemainigVentilationTime();
  } else {
    // FIXME: issue
    // Clear
    //lcd.clear();
  }
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

void heartbeat() {
  lcd.setCursor(10, 0);
  if (duration % 5 == 0) {
    lcd.print(" ");
  } else {
    lcd.printByte(1);
  }
}

// Temperature
void printTemperature() {
  lcd.setCursor(0, 0);
  lcd.print(temperature);
}


void printRemainigVentilationTime() {
  lcd.setCursor(1, 1);
  lcd.print(time.gettime("i:s")); 

  unsigned long seconds = int(millis() / 1000);
  lcd.setCursor(0, 1);
  if (seconds % 3 == 0) {
    lcd.print(" ");
  } else {
    lcd.printByte(3);
  }
}
