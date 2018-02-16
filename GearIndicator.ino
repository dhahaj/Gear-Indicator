//------------------------------------------
// Motorcycle Gear Indicator Source v1.3
// Created      : 8/4/2010
// Last Updated : 2/16/2018
//------------------------------------------

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

// Set to 1 to enable active low neutral input
#define NEUTRAL_ACTIVE_LOW 0

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

// Timing Configurations
#define DEBOUNCE_TIME 50
#define LOCK_TIME     175

// Specific Configurations
#define TOP_GEAR      9
#define SHOW_DIGITS(n) showDigit((n)); _delay_ms(575)

/*
 * Display a digit on the 7-segment display.
 */
void showDigit(uint8_t Digit) {
    uint8_t i;

    PORT |= (1 << RESET_BIT);       // RESET pin high
    PORT &= ~(1 << RESET_BIT);      // RESET pin low

    for (uint8_t i = 0; i < Digit; i++) {
        PORT |= (1 << CLOCK_BIT);   // CLOCK pin high
        PORT &= ~(1 << CLOCK_BIT);  // CLOCK pin low
    }
}

/*
 * Simple debouncing for the gear inputs
 */
int debouce(uint8_t pin) {
    if (bit_is_clear(PIN, pin)) {
        _delay_ms(DEBOUNCE_TIME);
        if (bit_is_set(PIN, pin)) return true;
    }
    return false;
}

volatile uint8_t gear;

/*
   Setup the function
*/
void setup(void) {

#if (NEUTRAL_ACTIVE_LOW)
    PORT = ( _BV(UP_BIT) | _BV(DOWN_BIT) | _BV(NEUTRAL_BIT) ); // Enable all pullups
#else
    PORT = ( _BV(UP_BIT) | _BV(DOWN_BIT) ); // UP and DOWN pin enable pull-up
#endif

    DDPORT |= _BV(RESET_DDR);      // RESET pin as output
    DDPORT |= _BV(CLOCK_DDR);      // CLOCK pin as output

    PORT &= ~(_BV(RESET_BIT));  // RESET pin low

    // Self test effect
    showDigit(0);
    _delay_ms(350);
    for (uint8_t i = 1; i < 10; i++)
        SHOW_DIGITS(i);

    showDigit(0);
    _delay_ms(500);

    gear = eeprom_read_byte((uint8_t*)0);  // Read initial value from EEPROM
    showDigit(gear);                       // Show initial value
}

/*
   MAIN LOOP
*/
void loop(void) {

    /***  UP SHIFT  ***/
    if (debouce(UP_BIT))
    {
        // Some logic here :-)
        (gear > 0 && gear < TOP_GEAR) ? gear++ : \
        (gear == 0) ? gear = 2 : NULL;
        // if ( (gear > 0) && (gear < TOP_GEAR) ) gear++;
        // if (gear == 0) gear = 2;

        uint8_t bit;
#if (NEUTRAL_ACTIVE_LOW)
        bit = !bit_is_clear(PIN, NEUTRAL_BIT);
#else
        bit = bit_is_clear(PIN, NEUTRAL_BIT);
#endif
        if (bit) gear = 0; // We're in neutral!

        eeprom_write_byte((uint8_t*)0, gear);
        showDigit(gear);
        _delay_ms(LOCK_TIME);
    }


    /***  DOWN SHIFT  ***/
    if (debouce(DOWN_BIT))
    {
        gear > 1 ? gear-- : \
        (gear == 0) ? gear=1 : NULL;
        // if (gear > 1) gear--;
        // if (gear == 0) gear = 1;

        uint8_t bit;
#if (NEUTRAL_ACTIVE_LOW)
        bit = !bit_is_clear(PIN, NEUTRAL_BIT);
#else
        bit = bit_is_clear(PIN, NEUTRAL_BIT);
#endif
        if (bit) gear = 0; // We're in neutral!

        eeprom_write_byte((uint8_t*)0, gear);
        showDigit(gear);
        _delay_ms(LOCK_TIME);
    }

}


