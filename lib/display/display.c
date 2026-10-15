#include "display.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#define SPACE 0xFF

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                               0x92, 0x82, 0xF8, 0X80, 0X90};

const uint8_t ALPHABET_MAP[] = {0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0xC2,
                                0x89, 0xCF, 0xE1, 0x8A, 0xC7, 0xEA, 0xC8,
                                0xC0, 0x8C, 0x4A, 0xCC, 0x92, 0x87, 0xC1,
                                0xC1, 0xD5, 0x89, 0x91, 0xA4};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay() {
  sbi(DDRD, LATCH_DIO);
  sbi(DDRD, CLK_DIO);
  sbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder) {
  uint8_t bit;

  for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
    if (bitorder == LSBFIRST) {
      bit = !!(val & (1 << i));
    } else {
      bit = !!(val & (1 << (7 - i)));
    }
    // write bit to register
    if (bit == HIGH)
      sbi(PORTB, DATA_DIO);
    else
      cbi(PORTB, DATA_DIO);

    // Trigger the clock pin so the display updates
    sbi(PORTD, CLK_DIO);
    cbi(PORTD, CLK_DIO);
  }
}

// Schrijft cijfer naar bepaald segment. Segment 0 is meest linkse.
void writeNumberToSegment(uint8_t segment, uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value], MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

// Schrijft getal tussen 0 en 9999 naar de display. Te gebruiken in een lus...
void writeNumber(int number) {
  if (number < 0 || number > 9999) return;
  writeNumberToSegment(0, number / 1000);
  writeNumberToSegment(1, (number / 100) % 10);
  writeNumberToSegment(2, (number / 10) % 10);
  writeNumberToSegment(3, number % 10);
}

// Schrijft getal tussen 0 en 9999 naar de display en zorgt dat het er een
// bepaald aantal milliseconden blijft staan. Opgelet: de timing is "ongeveer",
// er wordt geen rekening gehouden met de tijd writeNumberToSegment in beslag
// neemt...
void writeNumberAndWait(int number, int delay) {
  if (number < 0 || number > 9999) return;
  for (int i = 0; i < delay / 20; i++) {
    writeNumberToSegment(0, number / 1000);
    _delay_ms(4);
    writeNumberToSegment(1, (number / 100) % 10);
    _delay_ms(4);
    writeNumberToSegment(2, (number / 10) % 10);
    _delay_ms(4);
    writeNumberToSegment(3, number % 10);
    _delay_ms(4);
  }
}

void writeCharToSegment(uint8_t segment, char character) {
  if (character >= 'a' && character <= 'z') {  // kleine letter a..z omzetten naar hoofdletter
    character -= 32;
  }
  uint8_t value;
  if (character < 'A' || character > 'Z') {
    value = SPACE;
  } else {
    value = ALPHABET_MAP[character - 'A'];
  }
  cbi(PORTD, LATCH_DIO);
  shift(value, MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

// Schrijft de eerste 4 karakters van een string naar de display. Te gebruiken
// in een lus...
void writeString(char* str) {
  int lengte = strlen(str);
  if(lengte > 4) lengte = 4;
  for (int i = 0; i < lengte; i++) {
    writeCharToSegment(i, str[i]);
  }
}

// Schrijft string naar de display en zorgt dat het er een bepaald aantal
// milliseconden blijft staan.
void writeStringAndWait(char* str, int delay) {
  int lengte = strlen(str);
  if(lengte > 4) lengte = 4;
  for (int i = 0; i < delay / 20; i++) {
    for (int i = 0; i < lengte; i++) {
      writeCharToSegment(i, str[i]);
      _delay_ms(4);
    }
  }
}

void scrollingString(char* str, int delay){
  int length = 0;
  char character = str[0];
  while (character != '\0') {
    length++;
    character = str[length];
  }
  if (length <= NUMBER_OF_DISPLAYS){
    writeStringAndWait(str,delay);
  } else {
      int lower = 0;
      char* subStr = malloc (NUMBER_OF_DISPLAYS * sizeof(char));
      while (lower + NUMBER_OF_DISPLAYS <= length) {
        for (int i = 0; i < NUMBER_OF_DISPLAYS; i++){
          subStr[i] = str[lower + i];
        }
        writeStringAndWait(subStr,delay/(length+1-NUMBER_OF_DISPLAYS));
        lower++;
      }
  }
}


void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay) {
    for (int i = 0; i < delay / 20; i++) {
        writeNumberToSegment(3, seconden % 10);
        _delay_ms(4);
        writeNumberToSegment(2, seconden / 10);
        _delay_ms(4);
        writeNumberToSegment(1, minuten % 10);
        _delay_ms(4);
        writeNumberToSegment(0, minuten / 10);
        _delay_ms(4);
    }
}


void clearDisplay(){
  writeString("    ");
}

void writeBlankSegment(uint8_t segment) {
  cbi(PORTD, LATCH_DIO);
  shift(255, MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}