#define __DELAY_BACKWARD_COMPATIBLE__  // nodig, anders kan je geen variabelen
                                       // meegeven aan _delay_ms...
#include <avr/io.h>
#include <util/delay.h>

void enableLed(int lednumber) {
  if (lednumber < 0 || lednumber > 3) return;
  DDRB |= (1 << (PB2 + lednumber));
}

void enableLeds(uint8_t leds) {  // vb: 00001010 --> bit geeft lednummer aan,
                                 // komt dus niet overeen met de PORTB nummer!)
  leds &= 0b00001111;            // eerst bovenste 4 bits op 0 zetten
  DDRB |= (leds << PB2);
}

void enableAllLeds() {
  for (int i = 0; i < 4; i++) {
    enableLed(i);
  }
}

void lightUpLed(int lednumber) {
  if (lednumber < 0 || lednumber > 3) return;
  PORTB &= ~(1 << (PB2 + lednumber));
}

void lightUpLeds(uint8_t leds) {
  leds &= 0b00001111;
  PORTB &= ~(leds << PB2);
}

void lightUpAllLeds() {
  for (int i = 0; i < 4; i++) {
    lightUpLed(i);
  }
}

void lightDownLed(int lednumber) {
  if (lednumber < 0 || lednumber > 3) return;
  PORTB |= (1 << (PB2 + lednumber));
}

void lightDownLeds(uint8_t leds) {
  leds &= 0B00001111;
  PORTB |= (leds << PB2);
}

void lightDownAllLeds() {
  for (int i = 0; i < 4; i++) {
    lightDownLed(i);
  }
}

void dimLed(int lednumber, int percentage,
            int duration) {  // PWM: we gaan een tijdje aan en een tijdje uit
  if (lednumber < 0 || lednumber > 3) return;
  for (int i = 0; i < duration / 10; i++) {
    lightUpLed(lednumber);
    _delay_ms(percentage / 10);
    lightDownLed(lednumber);
    _delay_ms(10 - (percentage / 10));
  }
}

void dimLeds(uint8_t leds, int percentage,
             int duration) {  // PWM: we gaan een tijdje aan en een tijdje uit
  for (int i = 0; i < duration / 10; i++) {
    lightUpLeds(leds);
    _delay_ms(percentage / 10);
    lightDownLeds(leds);
    _delay_ms(10 - (percentage / 10));
  }
}

void fadeInLed(int led, int duration) {
  if (led < 0 || led > 3) return;
  for (int i = 0; i < 10; i++) {
    dimLed(led, i * 10, duration / 10);
  }
}

void fadeInLeds(uint8_t leds, int duration) {
  for (int i = 0; i < 10; i++) {
    dimLeds(leds, i * 10, duration / 10);
  }
}

void fadeOutLed(int led, int duration) {
  if (led < 0 || led > 3) return;
  for (int i = 9; i > 0; i--) {
    dimLed(led, i * 10, duration / 10);
  }
}

void fadeOutLeds(uint8_t leds, int duration) {
  for (int i = 9; i > 0; i--) {
    dimLeds(leds, i * 10, duration / 10);
  }
}

void flipLed(int led) {
  if (led < 0 || led > 3) return;
  if(PORTB & (1 << (PB2 + led))) { // staat UIT want er staat een 1
  //if (bit_is_set(PORTB, PB2 + led)) {
    lightUpLed(led);
  } else {
    lightDownLed(led);
  }
}

void flashLed(int lednr, int duration) {
  if (lednr < 0 || lednr > 3) return;
  lightUpLed(lednr);
  _delay_ms(duration);
  lightDownLed(lednr);
  _delay_ms(duration);
}
///////////////MorseCode///////////////
void beginMorse(){
for (int i = 4; i >= 0; i--){
    lightDownLed(i);
    _delay_ms(1000);
  }
}

void ledDans(){
    int aantal = 0;
    int aantal2 = 0;
    enableAllLeds();

    while (aantal < 5){
    lightDownAllLeds();
    _delay_ms(100);
    lightUpLed(0);
    _delay_ms(100);
    lightUpLed(1);
    _delay_ms(100);
    lightUpLed(2);
    _delay_ms(100);
    lightUpLed(3);
    _delay_ms(100);
    ///////////// Andere kant////////////
    lightDownLeds(0b00011100);
    _delay_ms(100);
    lightUpLed(3);
    _delay_ms(100);
    lightUpLed(2);
    _delay_ms(100);
    lightUpLed(1);
    _delay_ms(100);
    lightUpLed(0);
    _delay_ms(100);
    aantal++;
    }
    
    while (aantal2 < 4){
    lightDownAllLeds();
    _delay_ms(50);
    lightUpLed(0);
    lightUpLed(1);
    _delay_ms(500);
    lightDownAllLeds();
    _delay_ms(50);
    lightUpLed(2);
    lightUpLed(3);
    _delay_ms(500);
    aantal2++;
    }
    
    lightUpAllLeds();
    _delay_ms(1500);
    lightDownAllLeds();
    
}