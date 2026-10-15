#include <avr/io.h>

void enableBuzzer();
void buzzerOff();
void buzzerOn();
void playTone(float frequency, uint32_t duration);