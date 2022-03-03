#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include "defines.h"

LiquidCrystal_I2C lcd(0x27, 16, 4);
HX711 scale;

int mode = 0, lastMode = -1, pressedButton = -1, ext_command = 0;
int homeIndex = 0, loadIndex = 0;
int weight = 0, lastWeight = 0, weightDelta = 0, interruptCount = 0, alreadyIn = 0;
bool usrInteract = false, needRefresh = false, errorShown = false, glassRemovedFlag = false;
int tareMillisFlag = -1;


void setup() {
  Serial.begin(115200);

  initLCD();
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);

#ifndef DEVMODE
  showIntroScreen();
#else
  lcd.setCursor(0, 0);
  lcd.print("DEVMODE");
  delay(750);
  //lcd.clear();
#endif

  pinMode(LED, OUTPUT);
  for (int i : BUTTON) {
    pinMode(i, INPUT);
  }

  for (int i : RELAY) {
    pinMode(i, OUTPUT);
  }

  percToGram();

  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 1952;//3906;//7812;//15624;//31249;
  //OCR1B = 7812;
  TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12); //prescale 1024, CTC mode
  TIMSK1 |= (1 << OCIE1A);// | (1 << OCIE1B);*/

  // overeni pro spusteni s objektem na vaze (get bez units), preskoceni tlacitkem na boku
  weight = scale.get_units(10);
  //Serial.println(weight);
  if (weight >= INIT_WEIGHT_WARNING) {
    Serial.println(F("Objekt na vaze!"));
    lcd.setCursor(0, 0);
    lcd.print(F("Objekt na vaze"));
    lcd.setCursor(0, 1);
    lcd.print(F("Preskocit [X]"));
    while (!(weight >= INIT_WEIGHT_WARNING ^ digitalRead(BUTTON[4]))) {

    }
    if (!digitalRead(BUTTON[4])) {
      Serial.println(F("Tlacitkem"));
    } else {
      Serial.println(F("Sundano"));
    }
  }

  tareMillisFlag = millis() + 500;

}


void loop() {

  if (millis() >= tareMillisFlag) {
    scale.tare();
    tareMillisFlag = -1;
  }

  mode = checkMode();
  if (lastMode != mode) { //pokud se rezim zmenil
    disableAll();
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (mode) {
      case -1:
        disableTimer1();
        lcd.print(F("Chyba rezimu!"));
        break;

      case 0:
        disableTimer1();
        lcd.print(F("Nevybran rezim"));
        break;

      case 1:
        enableTimer1();
        lcd.print(REZIM[0]);
        break;

      case 2:
        lcd.print(REZIM[1]);
        delay(1500);
        homeIndex = 0;
        interruptCount = 0;
        enableTimer1();
        break;
    }
    lastMode = mode;
    needRefresh = true;
    homeIndex = 1;
  }

  if (mode == 1) { //manualni rezim
    if (glass()) {
      for (int i = 0; i < 4; i++) {
        digitalWrite(RELAY[i], digitalRead(BUTTON[i]));
      }
      if (isButtonPressed()) {
        enablePump(2);
      } else {
        enablePump(0);
      }
      if (errorShown) {
        lcd.setCursor(0, 1);
        lcd.print(empty);
        errorShown = false;
      }
    } else {
      disableAll();
      if (!errorShown) {
        lcd.setCursor(0, 1);
        lcd.print("VLOZTE SKLENICI");
        errorShown = true;
      }
    }
  } else if (mode == 2) {
    if (needRefresh) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Podrz [");
      lcd.setCursor(7, 0);
      lcd.print(homeIndex);
      lcd.setCursor(8, 0);
      lcd.print("] pro");
      lcd.setCursor(0, 1);
      lcd.print(PITI[homeIndex - 1]);
      needRefresh = false;
    }

    if (isButtonPressed() && !glass()) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("VLOZTE SKLENICI!");
      while (isButtonPressed()) {}
      delay(1000);
    } else {

      if(digitalRead(BUTTON[0])){
        pressedButton = 0;
      }else if(digitalRead(BUTTON[1])){
        pressedButton = 1;
      }else if(digitalRead(BUTTON[2])){
        pressedButton = 2;
      }else if(digitalRead(BUTTON[3])){
        pressedButton = 3;
      }else{
        pressedButton = -1;
      }

      if(ext_command){
        if(ext_command < 5){
          makeDrink(ext_command-1);
        }else{
          switch(ext_command){
            case 69: //google assistant
              makeDrink(0); 
              break;

            case 5: //zero calibrate
              scale.tare();
              percToGram();
              printMessage("ZERO_CAL", 0, 1000);
              break;

            case 6: //empty glass calibrate
              EMPTY_GLASS_WEIGHT = weight;
              percToGram();
              printMessage("EMPTY_CAL", EMPTY_GLASS_WEIGHT, 1000);
              break;

            case 7: //full glass calibrate
              FULL_GLASS_WEIGHT = weight;
              percToGram();
              printMessage("FULL_CAL", FULL_GLASS_WEIGHT, 1000);
              break;
          }
        }
        ext_command = 0;
      }


      if (pressedButton > -1) {
        usrInteract = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(PITI[pressedButton]);
        lcd.setCursor(0, 1);
        loadIndex = 1;
        while (digitalRead(BUTTON[pressedButton]) && glass()) {
          lcd.write(255);
          lcd.setCursor(loadIndex, 1);
          if (loadIndex <= 18) {
            loadIndex++;
          } else {
            makeDrink(pressedButton);
            delay(2000);
          }
          delay(70);
        }
        lcd.clear();
        usrInteract = false;
        needRefresh = true;
      }
    }
  }
}
