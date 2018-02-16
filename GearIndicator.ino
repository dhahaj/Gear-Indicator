//------------------------------------------
// Motorcycle Gear Indicator Source v1.3
// Created      : 8/4/2010
// Last Updated : 2/16/2018
//------------------------------------------

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

// PIN/PORT DEFINITIONS
#define PORT          PORTB
#define PIN           PINB
#define DDPORT        DDRB
#define RESET_BIT     PB0
#define CLOCK_BIT     PB1
#define NEUTRAL_BIT   PB2
#define UP_BIT        PB3
#define DOWN_BIT      PB4
#define RESET_DDR     DDB0
#define CLOCK_DDR     DDB1

// Timing Configiurtions
#define DEBOUNCE_TIME 50
#define LOCK_TIME     175

// Specific Configurations
#define TOP_GEAR      9
#define SHOW_DIGIT(n) showDigit((n)); _delay_ms(575);

volatile uint8_t gear;

void showDigit(uint8_t Digit) {
  uint8_t i;

  PORT |= (1 << RESET_BIT);       // RESET pin high
  PORT &= ~(1 << RESET_BIT);      // RESET pin low

  for (uint8_t i = 0; i < Digit; i++) {
    PORT |= (1 << CLOCK_BIT);   // CLOCK pin high
    PORT &= ~(1 << CLOCK_BIT);  // CLOCK pin low
  }
}

int debouce(uint8_t pin) {
  if (bit_is_clear(PIN, pin)) {
    _delay_ms(DEBOUNCE_TIME);
    if (bit_is_set(PIN, pin)) return true;
  }
  return false;
}

/*
   Setup the function
*/
void setup(void) {

  PORT = ( 1 << UP_BIT | 1 << DOWN_BIT ); // UP and DOWN pin enable pull-up
  DDPORT |= 1 << RESET_DDR;      // RESET pin as output
  DDPORT |= 1 << CLOCK_DDR;      // CLOCK pin as output

  PORT &= ~(1 << RESET_BIT);  // RESET pin low

  // Self test effect
  showDigit(0);
  _delay_ms(350);
  for (uint8_t i = 1; i < 10; i++) {
    SHOW_DIGIT(i)
  }

  showDigit(0);
  _delay_ms(500);

  gear = eeprom_read_byte((uint8_t*)0);  // Read initial value from EEPROM
  showDigit(gear);                       // Show initial value
}

/*
   MAIN LOOP
*/
void loop(void) {

  if (debouce(UP_BIT)) {
    if ( (gear > 0) && (gear < TOP_GEAR) ) gear++;
    if (gear == 0) gear = 2;
    if (!bit_is_clear(PIN, NEUTRAL_BIT)) gear = 0; // We're in neutal!

    eeprom_write_byte((uint8_t*)0, gear);
    showDigit(gear);
    _delay_ms(LOCK_TIME);
  }

  if (debouce(DOWN_BIT)) {
    if (gear > 1) gear--;
    if (gear == 0) gear = 1;
    if (!bit_is_clear(PIN, NEUTRAL_BIT)) gear = 0; // We're in neutal!

    eeprom_write_byte((uint8_t*)0, gear);
    showDigit(gear);
    _delay_ms(LOCK_TIME);
  }

}


