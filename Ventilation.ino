/**
 * Ventilation
 */
 
boolean ventilationOn;


void handleVentilation(float temperature, boolean turnOn) {
  turnOn = turnOn || temperature >= CRITICAL_HIGH_TEMPERATURE;
  
  if (ventilationOn != turnOn ){
    ventilationOn = turnOn;
    if (turnOn) {
      digitalWrite(VENTILATION_SLOT, HIGH);

      Serial.print(time.gettime("d-m-Y, H:i:s,"));
      Serial.println("  Start Ventilation");
    } else {
      digitalWrite(VENTILATION_SLOT, LOW);
      clearUserInfo();
      
      Serial.print(time.gettime("d-m-Y, H:i:s,"));
      Serial.println("  Stop Ventilation");
    }
  }
}

// Fift times per day
boolean isVentilation() {
  return time.day >= 2 && 
         time.Hours % 5 == 0 && 
         time.minutes <= getVentilationLength();
}


/**
  2-4 день пять разів на день охолодження 2-3 хв
  До 15 суток збільшити охолодження до 20 хв
  із 16 37.2 (без перевертання)
*/

int getVentilationLength() {
  return 0; // FIXME: debugger
  
  int day = time.day;
  const float multiplier = 4/3;
  int minutes = -1;
  if (day >= 2 && day <= 4 ) {
    minutes = 3;
  } else if (day > 4) {
    minutes = int(constrain(day*multiplier, 3, 20)) + 1;
  }
  
  return minutes;
}

