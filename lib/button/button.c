#include <avr/interrupt.h>
#include <avr/io.h>
#include <usart.h>
#include <util/delay.h>

void enableButton(int button){
    if (0 > button || button > 2)
        return;
    DDRC &= ~(1 << (PC1 + button));
    PORTC |= (1 << (PC1 + button));
}

void enableAllButtons() {
  for (int i = 0; i < 3; i++) {
    enableButton(i);
  }
}

int buttonPushed(int button) {
  return ((PINC & (1 << (PC1 + button))) == 0);
}

int buttonReleased(int button) { return !buttonPushed(button); }

void enableButtonInterrupt(int button) {
  if (button < 0 || button > 2) return;
  PCICR |= (1 << PCIE1);
  PCMSK1 |= 1 << (PC1 + button);
  sei();
}

void enableAllButtonInterrupts() {
  for (int i = 0; i < 3; i++) {
    enableButtonInterrupt(i);
  }
}


int buttonPushedDebounce (int button) {
    if (buttonPushed (button)) {
        _delay_us (2000);
        return buttonPushed (button);
    }
    return 0;
}

int buttonReleasedDebounce(int button){
    if (buttonReleased(button)){
        _delay_ms(1);
        return buttonReleased(button);
    }
    return 0;
}

uint8_t debounce(uint8_t pin, uint8_t button) {
    if (bit_is_clear(pin, button)) {
        _delay_us(2000);
        if (bit_is_clear(pin, button))
            return 1;
    }
    return 0;
}

void initButtons(void) {
    PCICR |= (1 << PCIE1);
    PCMSK1 |= ((1 << PC1) | (1 << PC2) | (1 << PC3));
}