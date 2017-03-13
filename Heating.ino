const int CRITICAL_HIGH_TEMPERATURE = 40;
const int CRITICAL_LOW_TEMPERATURE = 25;

float deltaT = 0;
boolean heatingFlag;

/** Temperature
Modes:
  1-12 day 36.6-37.7
  13-15 day 37.3-37.5
  із 16 day 37.2
*/
void validateTemparature(float temperature, boolean ventilation) {
  if (ventilation) {
    heating(false);
  } else if (temperature < getMinTemperature(true)) {
    heating(true);
  } else if (temperature >= getMaxTemperature(true)) {
    heating(false);
  } 

  boolean isCriticalValue = (temperature <= CRITICAL_LOW_TEMPERATURE && !ventilation) || temperature >= CRITICAL_HIGH_TEMPERATURE;
  if (isCriticalValue) {
    // PROD: alarm(); 
  }
}

void heating(boolean turnOn) {
  if (turnOn) blinkHeating(); //Display.h
  
  if (heatingFlag != turnOn) {
    heatingFlag = turnOn;
    if (turnOn) {
      digitalWrite(POWER_SLOT, HIGH);  
    } else {
      digitalWrite(POWER_SLOT, LOW);
      clearUserInfo();
    }
  }
}

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
