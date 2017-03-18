const int CRITICAL_HIGH_TEMPERATURE = 40;
const int CRITICAL_LOW_TEMPERATURE = 25;

float deltaT = 0;
boolean heatingFlag;


boolean validateTemparature(float temperature, boolean ventilation) {

  temperature += deltaT; // !Apply

  if (ventilation) {
    return heating(false, temperature);
  } else if (temperature < getMinTemperature()) {
    return heating(true, temperature);
  } else if (temperature >= getMaxTemperature()) {
    return heating(false, temperature);
  } 

  boolean isCriticalValue = (temperature <= CRITICAL_LOW_TEMPERATURE && !ventilation) || temperature >= CRITICAL_HIGH_TEMPERATURE;
  if (isCriticalValue && PROD) {
    alarm();
  }

  return false;
}

boolean heating(boolean turnOn, float t) {
  if (heatingFlag != turnOn) {
    heatingFlag = turnOn;
    if (turnOn) {
      digitalWrite(POWER_SLOT, HIGH); 
      // Logger
      Serial.print(time.gettime("d-m-Y, H:i:s,"));
      Serial.print("  Start Heation, t = ");
      Serial.println(t);
    } else {
      digitalWrite(POWER_SLOT, LOW);
      // Logger
      Serial.print(time.gettime("d-m-Y, H:i:s,"));
      Serial.print("  Stop Heation, t = ");
      Serial.println(t);
    }
  }
  return turnOn;
}

/** Limits
  ---- Quail / Перепілки ---
  1-12 day 36.6-37.7
  13-15 day 37.3-37.5
  із 16 day 37.2

  ---- Chicken / Курка ---
  1-18: t37.5 - 37.6, 50-60% (37.5/ 45%)
  18: t37.2, 80% (37.0 / 60%)
*/

float getMinTemperature() {
  byte day =  time.day;
  return CHICKEN_MODE
    ? getChickenMin(day)
    : getQuailMin(day);
}

float getMaxTemperature() {
  byte day =  time.day;
  return CHICKEN_MODE
    ? getChickenMax(day)
    : getQuailMax(day);
}


// -- Chicken --
float getChickenMin(byte day) {
    return (day < 18)
        ? 37.5
        : 37.2;
}

float getChickenMax(byte day) {
    return (day < 18)
        ? 37.6
        : 37.2;
}

// -- Quail --
float getQuailMin(byte day) {
  if (day <= 12) {
    return 36.6;
  } else if (day >= 13 && day <= 15 ) {
    return 37.3;
  } else {
    return 37.2;
  }
}

float getQuailMax(byte day) {
  if (day <= 12) {
    return 37.7;
  } else if (day >= 13 && day <= 15 ) {
    return 37.5;
  } else {
    return 37.2;
  }
}
