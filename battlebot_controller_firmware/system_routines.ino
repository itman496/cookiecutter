void lcdcontrolcallback() {
  
  int x = millis();
  windowmanager();
  x = millis() - x;
  Serial.print("DISPLAY ROUTINE ELAPSED TIME: ");
  Serial.print(x);
  Serial.println("ms");
}

void inputcontrolcallback() {
  strafeVar = (constrain(map(analogRead(JOY1_INPUT),154,872,-255,255),-255,255))*-1;  //read joysticks
  if(strafeVar > -deadZone && strafeVar < deadZone) {  //deadzone for center
    strafeVar = 0;
  }
  moveVar = constrain(map(analogRead(JOY0_INPUT),217,859,-255,255),-255,255);
  if(moveVar > -deadZone && moveVar < deadZone) {  //deadzone for center
    moveVar = 0;
  }
  yawVar = constrain(map(analogRead(JOY3_INPUT),180,886,-255,255),-255,255);
  if(yawVar > -deadZone && yawVar < deadZone) {  //deadzone for center
    yawVar = 0; 
  }
  throttleVar = constrain(map(analogRead(JOY2_INPUT),830,134,0,512),-0,512);
  if(throttleVar < 30) {  //deadzone for bottom
    throttleVar = 0;
  }
  
  L1Var = !digitalRead(BTN2_INPUT);  //read buttons
  L2Var = !digitalRead(BTN3_INPUT);
  R1Var = !digitalRead(BTN0_INPUT);
  R2Var = !digitalRead(BTN1_INPUT);


  digitalWrite(VIBRATE_OUT,L1Var);  //test vibrate with L1

  if(R1Var == 1) { //test IR beacon with R1
    tone(IR_PWM_OUT, IR_PWM_FREQ);
  }
  else {
    noTone(IR_PWM_OUT);
  }

}

void batterymonitorcallback() {
  packVoltage = lc.cellVoltage();
  packPercent = lc.cellPercent();
  /*
  Serial.print("CELL VOLTAGE: ");
  Serial.print(packVoltage);
  Serial.println("");
  Serial.print("CELL PERCENT: ");
  Serial.print(packPercent);
  Serial.println("");
  */
}
void windowmanager () {  //window manager state machine
  
  receiveroutine();  //receive radio packet

  layerflipper();  //flips the screen and active layer to make flickering go away  
  tft.clearScreen();  //clear screen for new drawing
  touchroutine();  //check touchscreen stuff

  radio.receiveDone(); //test to wake radio spi up after coming off SPI from another device
  delay(1);
  transmitroutine(); //inserting this after display stuff, layer flipper and touchscreen take about 9ms to complete
  radio.receiveDone(); //test to put radio back in receive mode maybe?
  
  tft.setFontScale(1);  //dummy command to wake the chip up after coming off SPI from another device
  
  switch (screenNumber) {  //draw whatever is needed to screen
  case 1:    
    robotstatusscreen();
    break;
  case 2:
    radiosettingsscreen();
    break;
  case 3:
    robotsettingsscreen();
    break;
  case 4:
    robotselectscreen();
    break;
  default:
    testscreen();
    break;
  }
  tft.setCursor(800,480);  //move cursor out of window for whatever, it stops an E being added to the last thing written..  I hate this stupid display controllerE.. (haha get it)
}

void buttonrouter(int buttonNumber) {
  switch (buttonNumber) {  //see what button was pressed
  case 101:  //btn 101: robot status
    screenNumber = 1;
    break;
  case 201:  //btn 201: radio settings
    screenNumber = 2;
    break;
  case 301:  //btn 301: robot settings
    screenNumber = 3;
    break;
  case 401:  //btn 401: robot select
    screenNumber = 4;
    break;
  case 901:  //btn 901: test int +10
    cmdValue = 1;  //issue command 1
    cmdVarType = 1;  //type is int (1)
    sentVarInt = 10;
    break;
  case 902:  //btn 902: test int -100
    cmdValue = 1;  //issue command 1
    cmdVarType = 1;  //type is int (1)
    sentVarInt = -100;
    break;
  default:  //invalid button pressed
    //screenNumber = 0;
    break;
  }
}

void layerflipper() {
  tft.setFontScale(1);  //dummy command to wake the chip up after coming off SPI
  if(drawLayer == false) {  //draw to layer 1 if false
    tft.writeTo(L1);  //change to layer L1
    tft.layerEffect(LAYER2);  //show layer L2
    delayMicroseconds(layerSwitchDelay);  //delay for RA8875 to get its shit together
  }
  else {                    //draw to layer 2 if true
    tft.writeTo(L2);  //change to layer L2
    tft.layerEffect(LAYER1);  //show layer L1
    delayMicroseconds(layerSwitchDelay);  //delay for RA8875 to get its shit together
  }
  drawLayer = !drawLayer;  //flip draw layer variable
}
