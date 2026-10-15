void enableLed(int lednumber);
void enableLeds(uint8_t leds); 
void enableAllLeds();

void lightUpLed(int lednumber);
void lightUpLeds(uint8_t leds);
void lightUpAllLeds();

void lightDownLed(int lednumber);
void lightDownAllLeds();
void lightDownLeds(uint8_t leds);

void dimLed(int lednumber, int percentage, int duration);
void dimLeds(uint8_t leds, int percentage, int duration); 
void fadeInLed(int led, int duration);
void fadeInLeds(uint8_t leds, int duration);
void fadeOutLed(int led, int duration);
void fadeOutLeds(uint8_t leds, int duration);

void flipLed(int led);
void flashLed(int led, int duration);
void beginMorse();
void ledDans();