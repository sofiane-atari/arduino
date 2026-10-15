#include <util/delay.h>
#include <avr/io.h>

void enablePotentiometer(){
    ADMUX |= (1 << REFS0); 
    ADCSRA |= (1 <<ADPS2) | (1 << ADPS1 ) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN);
}

uint16_t readPotentiometer() {
    ADCSRA |= (1<<ADSC); //zet bit op 1 en start de adc conversie
    loop_until_bit_is_clear(ADCSRA,ADSC);//adc start niet direct dus wachten op sample rate
    uint16_t value = ADC;//geeft het getal dat waar de potentiometer op staat
    return value;
}