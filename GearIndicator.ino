//------------------------------------------
// Motorcycle Gear Indicator Source v1.4
// Created      : 8/4/2010
// Last Updated : 2/19/2018
//------------------------------------------

#include <EEPROM.h>

// Uncomment to enable an active high neutral input signal. Else,
// the default assumption is that the neutral signal switches to ground
//#define NEUTRAL_ACTIVE_HIGH 
//#define SERIAL_DEBUG(x) (Serial.println((x)))

#define EE_ADDR 0

// Timing Configurations
#define DEBOUNCE_TIME 60
#define LOCK_TIME     175
#define TOP_GEAR      0x0F

volatile uint8_t gear;
uint8_t reset = 0,
        clock = 1,
        up = 3,
        down = 4,
        neutral = 2;
//uint8_t reset = 13,
//        clock = 12,
//        up = 2,
//        down = 3,
//        neutral = 4;

/*
   Display a digit on the 7-segment display.
*/
void showDigit(uint8_t digit) {
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  for (uint8_t i = 0; i < digit; i++)
    digitalWrite(clock, !digitalRead(clock));
}

/*
   Simple debouncing for the gear inputs
*/
bool debouce(uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    delay(DEBOUNCE_TIME);
    if (digitalRead(pin) == HIGH) return true;
  }
  return false;
}

void setup(void) {

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  SERIAL_DEBUG("Gear Indicator Sketch begining..");
#endif

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);

  // Do not use a pullup if we're going to use an active high input
#ifdef NEUTRAL_ACTIVE_HIGH
  pinMode(neutral, INPUT);
#else
  pinMode(neutral, INPUT_PULLUP);
#endif
  pinMode(reset, OUTPUT);
  pinMode(clock, OUTPUT);
  digitalWrite(reset, LOW);

  selfTest();

  gear = readGear(); // Read initial value from EEPROM
  showDigit(gear);
}

/*
   MAIN LOOP
*/
void loop(void) {

  /***  UP SHIFT  ***/
  if (debouce(up))
  {
    switch (gear) {
      case 0:
        gear = 2;
        break;
      default:
        if (gear < TOP_GEAR)
          gear++;
        break;
    }
    if (inNeutral()) gear = 0;
    showDigit(gear);
    saveGear(gear);
  }

  /***  DOWN SHIFT  ***/
  if (debouce(down))
  {
    switch (gear) {
      case 0:
        gear = 1;
        break;
      case 1:
        break;
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        if (gear < TOP_GEAR)
          gear--;
        break;
      default:
        return;
    }
    if (inNeutral()) gear = 0;
    showDigit(gear);
    saveGear(gear);
  }

}

/*
   Saves the current gear to the eeprom.
*/
void saveGear(uint8_t g) {
#ifdef SERIAL_DEBUG
  SERIAL_DEBUG("saving gear: ");
  SERIAL_DEBUG(g);
#endif
  EEPROM.update(EE_ADDR, g);
  delay(LOCK_TIME);
}

/*
   Reads the stored gear from the eeprom. Also handles
   cases where the value is unset.
*/
uint8_t readGear(void) {
  uint8_t val;
  EEPROM.get(EE_ADDR, val);
#ifdef SERIAL_DEBUG
  SERIAL_DEBUG("reading gear: ");
  SERIAL_DEBUG(val);
#endif
  if (val > TOP_GEAR) {
    delay(1);
    saveGear(0);
    return 0;
  }
  return val;
}

/*
   Returns true if the neutral signal is active.
*/
boolean inNeutral(void) {
#ifdef NEUTRAL_ACTIVE_HIGH
  return (digitalRead(neutral) == LOW);
#else
  return (digitalRead(neutral) == HIGH);
#endif
}

/*
   Run a display self test.
*/
void selfTest(void) {
  showDigit(0);
  delay(350);
#ifdef SERIAL_DEBUG
  SERIAL_DEBUG("Self Testing");
#endif
  for (int i = 1; i < 10; i++) {
#ifdef SERIAL_DEBUG
    SERIAL_DEBUG(i);
#endif
    showDigit(i);
    delay(500);
  }
  showDigit(0);
  delay(500);
}


