#include "display.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

void startTimer() {
    TCCR2B |= _BV(CS22) | _BV(CS21);  // --> prescalerfactor is nu 256 (telsnelheid = elke 16 µs)
}

void stopTimer() {
    TCCR2B = 0;  //--> prescaler 0 stopt de timer
}

void initTimer() {
    // STAP 1: kies de WAVE FORM en dus de Mode of Operation
    // Hier kiezen we CTC zodat TCNT2 steeds tot ORC2A telt en dan reset
    TCCR2A |= _BV(WGM21);

    // STAP 3: interrupts enablen
    TIMSK2 |= _BV(OCIE2A);  // OCRA interrupt enablen
    OCR2A = 250;
}