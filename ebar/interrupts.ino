ISR(TIMER1_COMPA_vect) {
  digitalWrite(LED, HIGH);
  lastWeight = weight;
  weight = scale.get_units(2);
  weightDelta = weight - lastWeight;
  /*Serial.print(weight);
  Serial.print(";");
  Serial.print(lastWeight);
  Serial.print(";");
  Serial.print(weightDelta);
  Serial.println("");*/
  
  if (interruptCount >= 16) {
    if (!usrInteract && mode == 2) {
      if (homeIndex == SIZE(PITI)) {
        homeIndex = 1;
      } else {
        homeIndex++;
      }
      needRefresh = true;
      interruptCount = 0;
      //Serial.println(homeIndex);
    } else {
      //Serial.println("busy");
    }
  } else {
    interruptCount++;
  }
  digitalWrite(LED, LOW);
}

/*ISR(TIMER1_COMPB_vect){
  //Serial.println(scale.get_units(2));
  Serial.println("druhej ted");
  }*/
