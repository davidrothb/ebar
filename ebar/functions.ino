void showIntroScreen() {
  lcd.setCursor(0, 0);
  customE(0);
  customB(4);
  customA(8);
  customR(12);
  delay(1500);
  lcd.setCursor(0, 0);
  lcd.print("Made by:" + empty);
  for (String s : JMENA) {
    lcd.setCursor(0, 1);
    lcd.print(s + empty);
    delay(1500);
  }
  lcd.clear();
}

int checkMode() {
  if (digitalRead(BUTTON[5]) == HIGH && digitalRead(BUTTON[6]) == LOW) { //manualni, pozice vlevo
    return 1;
  } else if (digitalRead(BUTTON[5]) == LOW && digitalRead(BUTTON[6]) == HIGH) { //automaticky, pozice vpravo
    return 2;
  } else if (digitalRead(BUTTON[5]) == LOW && digitalRead(BUTTON[6]) == LOW) { //nevybrano
    return 0;
  } else if (digitalRead(BUTTON[5]) == LOW && digitalRead(BUTTON[6]) == HIGH) { //dojebano
    return -1;
  }
}

bool safeToPour() {
  return true;
}

void disableAll() {
  enablePump(0);
  for (int i = 0; i < 4; i++) {
    digitalWrite(RELAY[i], LOW);
  }
}

void enablePump(int i) {
  switch (i) {
    case 0:
      digitalWrite(RELAY[4], LOW);
      digitalWrite(RELAY[5], LOW);
      break;

    case 1:
      digitalWrite(RELAY[4], LOW);
      digitalWrite(RELAY[5], HIGH);
      break;

    case 2:
      digitalWrite(RELAY[4], HIGH);
      digitalWrite(RELAY[5], HIGH);
      break;
  }
}

bool isButtonPressed() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(BUTTON[i])) {
      return true;
    }
  }
  return false;
}

bool glass() {
  return weight >= EMPTY_GLASS_WEIGHT / 2;
}

void enableTimer1() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12); //prescale 1024, CTC mode
}

void disableTimer1() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (0 << CS12) | (0 << CS10);
}

void percToGram() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grams[i][j] = ((float)percent[i][j]/100*MAX_LIQUID_WEIGHT);
    }
  }
}


void pourAmountFrom(int startWeight, int amount, int n) {
  if(amount == 0){
    return;
  }else{
    if(startWeight != 0 && amount != 0){
    delay(1000);
    }
  }
  //Serial.println(amount);
  digitalWrite(RELAY[n], HIGH);
  enablePump(2);
  while ((weight - EMPTY_GLASS_WEIGHT - startWeight) <= (amount - OVERSHOOT)) {
    Serial.println(weight - EMPTY_GLASS_WEIGHT - startWeight);
    if (!glass()) {
      Serial.println(F("odebrana sklenice!"));
      glassRemovedFlag = true;
      break;
    }
    //Serial.println(weight - EMPTY_GLASS_WEIGHT);
  }
  disableAll();
}

void receiveEvent(int num_bytes)
{
  ext_command = Wire.read();
  Serial.print("I2C received byte: ");
  Serial.println(ext_command);
}

void makeDrink(int pressedButton){
  lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("Priprava napoje"));
            alreadyIn = 0;
            for (int i = 0; i < 4; i++) {
              pourAmountFrom(alreadyIn, grams[pressedButton][i], i);
              alreadyIn += grams[pressedButton][i];
              if (glassRemovedFlag) {
                break;
              }
            }
            if (glassRemovedFlag) {
              glassRemovedFlag = false;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("SKLENICE");
              lcd.setCursor(0, 1);
              lcd.print("ODEBRANA");
            } else {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Odeberte napoj");
            }
            ext_command = 0;
}

void printMessage(String text, int num, int delayTime){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  lcd.setCursor(0,1);
  lcd.print(num);
  delay(delayTime);
}
