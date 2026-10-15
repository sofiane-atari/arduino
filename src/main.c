#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <usart.h>
#include <display.h>
#include <button.h>
#include <buzzer.h>
#include <led.h>
#include <potentio.h>
#include <timer.h>
#include <string.h>



#define MAX_RONDES 10

//Hoelang een tone moet duren
#define DURATION 250

//Frequenties voor de Buzzer
#define  C5      523.250
#define  D5      587.330
#define  E5      659.250
#define  F5      698.460
#define  G5      783.990
#define  A5      880.000
#define  B5      987.770
#define  C6      1046.500

long counter = 0;                   //timer
uint8_t seconden = 0;               //timer
uint8_t minuten;                    //timer

int aantalKeerGedrukt = 0;          //teller

int timerAan = 0;                   //boolean
int s1MagIngedrukWorden = 0;        //boolean  
int eindeLus = 0;                   //boolean
int s1NietIngedrukt = 0;            //boolean 

char naam[100];                     //Om naam in te lezen

// Deze ISR loopt elke 4ms
ISR(TIMER2_COMPA_vect) {
  counter++;
  if (counter == 250) {  // 1 second has elapsed
    if (seconden <= 0) {
      minuten--;
      seconden = 60;
    }
    seconden--;
    counter = 0;
  }
}

void initEnEnableAlles(){
    initUSART();
    initDisplay();
    initButtons();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    enableAllButtonInterrupts();
    initTimer();
    enableBuzzer();
    enablePotentiometer();
    clearDisplay();
    sei();
}

typedef struct {
  char* naam;
  int score;
  uint8_t minuten;
  uint8_t seconden;
} PLAYER;

PLAYER* players[MAX_RONDES];

char* getNaam(int index){
  return players[index]->naam;
}

int getScore(int index){
  return players[index]->score;
}

uint8_t getMinuten(int index){
  return players[index]->minuten;
}

uint8_t getSeconden(int index){
  return players[index]->seconden;
}

void setMinuten(int index){
  players[index]->minuten = minuten;
}

void setSeconden(int index){
  players[index]->seconden = seconden;
}

void setScore(int index){
  players[index]->score = aantalKeerGedrukt;
}

void setNaam(int index){
  players[index] = malloc(sizeof(players));
  players[index]->naam = malloc(strlen(naam) + 1);
  strcpy(players[index]->naam, naam);
}

void resetBooleans(){
  aantalKeerGedrukt = 0;
  s1MagIngedrukWorden = 0;
  timerAan = 0;
  eindeLus = 0;
  s1NietIngedrukt = 0;
}

void lampjesTimerAan(int* waardePotentiometer){
    if(*waardePotentiometer >= 30) lightUpLed(3);
    if(*waardePotentiometer >= 60) lightUpLed(2);
    if(*waardePotentiometer >= 90) lightUpLed(1);
    if(*waardePotentiometer >= 120) lightUpLed(0);
}

void lampjesTimerUit(uint8_t* aantalMinuten){
  int min = *aantalMinuten * 60;
  if(min + seconden < 120) lightDownLed(0);
  if(min + seconden < 90) lightDownLed(1);
  if(min + seconden < 60) lightDownLed(2);
  if(min + seconden < 30) lightDownLed(3);
}

void leesPotentio(){
  while(!s1NietIngedrukt){
    int waarde = readPotentiometer();
    if(waarde > 120) waarde = 120;
    minuten = waarde / 60;
    seconden = waarde % 60;
    lampjesTimerAan(&waarde);
    lampjesTimerUit(&minuten);
    writeTimeAndWait(minuten, seconden, 250);
  } 
}

// Functie die opgeroepen wordt door S2
void toonAantalKeerGedrukt() {
  printf("\nJe drukte op de 2de knop, de timer is gepauzeerd.\nOm de timer terug te laten lopen druk je op de 1ste knop.");
  for (int i = 0; i < 3; i++){
      clearDisplay();
      playTone(C5, DURATION);
      _delay_ms(DURATION);
    }

    while (buttonReleasedDebounce(0)){
      writeNumberAndWait(aantalKeerGedrukt,100);
    }
}

// Deze ISR is er voor de knopjes
ISR(PCINT1_vect) {
    // S1 start de teller
    if (buttonPushedDebounce(0) && s1MagIngedrukWorden) {
        if(!timerAan) printf("\nJe drukte op de 1ste knop, de timer begint te lopen.");
        startTimer();
        timerAan = 1;
        s1NietIngedrukt = 1 ;
    }

    // S2 stopt de teller + toont aantalkeergedrukt op display
    if (buttonPushedDebounce(1) && !eindeLus) {
        if(timerAan){
          stopTimer();
          timerAan = 0;
          toonAantalKeerGedrukt();
        }
    }

    // S3 telt aantal keer gedrukt
    if (buttonPushedDebounce(2) && !eindeLus) {
      if (timerAan){ aantalKeerGedrukt++;}
    }
}

void drukSepelerInfoAf(int aantal){
    printf("\nSpelers Info: \n");
  for (int i = 0; i < aantal; i++){
    printf("De %d° speler zijn naam was: %s en had een score van %d binnen de %d minuten en %d seconden.\n",i+1,getNaam(i),getScore(i),getMinuten(i),getSeconden(i));
  }
}

void einde(){
  lightUpAllLeds();
  uint32_t frequencies[] = {C5, D5, E5, F5, G5, A5, B5, C6};
  
  clearDisplay();
   for (int note = 0; note < 8; note++) {
    playTone(frequencies[note], DURATION);
    _delay_ms(DURATION);
  }

  for (int note = sizeof(frequencies) / sizeof(frequencies[0])-1; note >= 0; note--) {
    playTone(frequencies[note], DURATION);
    _delay_ms(DURATION);
  }

  scrollingString("Het spel is afgelopen", 6500);
  clearDisplay();
  _delay_ms(1500);
  scrollingString("Aantal keer gedrukt: ",6500);
  writeNumberAndWait(aantalKeerGedrukt,2500);

  for (int j = 0; j < 30; j++){
    clearDisplay();
    int getal = rand() % 4;
    lightUpLed(getal);
    _delay_ms(100);
    lightDownLed(getal);
    _delay_ms(100);
    }
  lightDownAllLeds();
}

void berekenHoogsteScore(){
  PLAYER player;
  player.score = 0;
  player.naam ="";
  player.seconden = 0;
  player.minuten = 0;

  for (int i = 0; i < MAX_RONDES; i++){
    if (getScore(i) > player.score){
      player.score = getScore(i);
      strcpy(player.naam, getNaam(i));
      player.seconden = getSeconden(i);
      player.minuten = getMinuten(i);

    } else if (getScore(i) == player.score){
      if (getMinuten(i) + getSeconden(i) < player.minuten + player.seconden){
        strcpy(player.naam, getNaam(i));
        player.score = getScore(i);
      }
        strcpy(player.naam, getNaam(i));
        player.score = getScore(i);
    }
  }
  if (player.score == 0){ printf("\nJammer %s, je hebt een score gehaalt van %d!!",player.naam,player.score);}
  else printf("\nGEFELICITEERD %s, je hebt de game gewonnen met een Score van %d!!",player.naam,player.score);
}

void vrijGevenArraySpelers(PLAYER *arrayPlayers[MAX_RONDES]){
  for (int i = 0; i < MAX_RONDES; i++) {
    free(arrayPlayers[i]->naam);
  }
}

int main() {
    initEnEnableAlles();
    for (int i = 0; i < MAX_RONDES; i++){
      printf("\nRonde: %d\n",i+1);
      printf("\nVoordat we beginnen aan het spel, gelieve je naam in de geven: ");
      readString(naam, 100);
      receiveByte();
      malloc(strlen(naam) * sizeof(char) + 1);
      setNaam(i);

      printf("\nDag %s, draai aan de potentiometer om een bepaalde tijd te geven aan de timer (max 2 minuten).\n", getNaam(i));
      _delay_ms(1500);
    
      printf("\nStart de timer met de 1ste knop\nStop de timer met de 2de knop (hier zie je ook het aantal keer dat je hebt gedrukt met knop 3)\nEn druk zoveel mogelijk op de 3de knop.\n");
      s1MagIngedrukWorden = 1;
      leesPotentio();
      setMinuten(i);
      setSeconden(i);

      printf("\nDe tijd die je hebt gekozen is %d minuten en %d seconden.\n",getMinuten(i), getSeconden(i));
      while(minuten + seconden > 0){
      writeTimeAndWait(minuten, seconden, 250);
      lampjesTimerUit(&minuten);
      }

      setScore(i);
      if (getScore(i) >= (seconden + minuten*60)){printf("\n\nGefeliciteerd %s, met je prachtige score van %d!\n", getNaam(i), getScore(i));}
      else printf("\n\nJammer %s, je behaalde een score van %d!\n", getNaam(i), getScore(i));

      for (int i = 0; i < 237; i++){
        printf("-");
      }
      
      eindeLus = 1;      
      einde();
      resetBooleans();
    }
    drukSepelerInfoAf(MAX_RONDES);
    berekenHoogsteScore();
    vrijGevenArraySpelers(&players[MAX_RONDES]);
    return 0;
}