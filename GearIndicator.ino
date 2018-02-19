//------------------------------------------
// Motorcycle Gear Indicator Source v1.3
// Created      : 8/4/2010
// Last Updated : 2/16/2018
//------------------------------------------

#include <avr/io.h>
#include <avr/eeprom.h>
#include <EEPROM.h>

// Uncomment to enable active high neutral input (+12V signal)
//  else neutral will be active when the pin is pulled to ground.

#define NEUTRAL_ACTIVE_HIGH 0
#define NEUTRAL_ACTIVE_LOW 1

// PIN/PORT DEFINITIONS
#define PORT          PORTB
#define PIN           PINB
#define DDPORT        DDRB
#define RESET_BIT     0 //PB0
#define CLOCK_BIT     1 //PB1
#define NEUTRAL_BIT   2 //PB2
#define UP_BIT        3 // PB3
#define DOWN_BIT      4 //PB4
#define RESET_DDR     DDB0
#define CLOCK_DDR     DDB1
#define EE_ADDR       0 //((uint8_t*)0)

// Timing Configurations
#define DEBOUNCE_TIME 50
#define LOCK_TIME     175

// Specific Configurations
#define TOP_GEAR      9
#define SHOW_DIGITS(n) showDigit((n)); delay(575)

volatile uint8_t gear;
int reset = 0, clock = 1, up = 3, down = 4, neutral = 5;

/*
   Display a digit on the 7-segment display.
*/
void showDigit(uint8_t digit) {
  digitalWrite(reset, HIGH);
  delay(1);
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

/*
   Setup the function
*/
void setup(void) {

  // Enable pullups
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
#ifndef NEUTRAL_ACTIVE_HIGH // Include the neutral input pin if it is active low
  pinMode(neutral, INPUT_PULLUP);
#else
  pinMode(neutral, INPUT);
#endif

  pinMode(reset, OUTPUT);
  pinMode(clock, OUTPUT);
  digitalWrite(reset, LOW);

  selfTest();

  gear = eeprom_read_byte(EE_ADDR); // Read initial value from EEPROM
  showDigit(gear);  // Show initial value
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
  if (debouce(DOWN_BIT))
  {
    switch (gear) {
      case 0:
        gear = 1;
        break;
      default:
        if (gear < TOP_GEAR)
          gear--;
        break;
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
  EEPROM.update(EE_ADDR, g);
  delay(LOCK_TIME);
}

/*
   Returns true if the neutral signal is active.
*/
boolean inNeutral(void) {
#if (NEUTRAL_ACTIVE_HIGH & NEUTRAL_ACTIVE_LOW)
#error "Conflicting values for active high/low neutral input!"
#elif not(NEUTRAL_ACTIVE_HIGH | NEUTRAL_ACTIVE_LOW)
#error "No definitions for active high or low neutral input!"
#endif

#if (NEUTRAL_ACTIVE_HIGH)
  return (digitalRead(neutral) == LOW);
#elif (NEUTRAL_ACTIVE_LOW)
  return (digitalRead(neutral) == HIGH);
#endif

}

/*
   Run a display self test.
*/
void selfTest(void) {
  showDigit(0);
  delay(350);
  for (byte i = 1; i < 10; i++) {
    showDigit(i);
    delay(500);
  }
  showDigit(0);
  delay(500);
}


