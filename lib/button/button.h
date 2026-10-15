void enableButton(int button);
int buttonPushed(int button);
int buttonReleased(int button);
void enableAllButtons();
void enableButtonInterrupt(int button);
void enableAllButtonInterrupts();

int buttonPushedDebounce(int button);
int buttonReleasedDebounce(int button);

uint8_t debounce(uint8_t pin, uint8_t button);
void initButtons(void);