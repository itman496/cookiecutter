void accelreadcallback() {
  read_accel();
}

void voltagereadcallback() {
  read_voltages();
}

void read_accel() {  //read data from accelerometer
  int16_t x,y,z;
  accel.readXYZ(&x,&y,&z);

  filteredRPM = f.filterIn(x);
  xAvg.reading(x);
  yAvg.reading(y);
  zAvg.reading(z);
  //Serial.print("X RAW: ");  Serial.println(event.acceleration.x);
  //xAvg.add(event.acceleration.x);
  //yAvg.add(event.acceleration.y);
  //zAvg.add(event.acceleration.z);
}


void read_voltages() {  //read ADC voltages and convert
  VOLTS_BAT = mapfloat(analogRead(ADC_VIN), 0.0, 1023.0, 0.0, 70.3);
  VOLTS_5V = mapfloat(analogRead(ADC_5V), 0.0, 1023.0, 0.0, 24.1);
  VOLTS_33V = mapfloat(analogRead(ADC_33V), 0.0, 1023.0, 0.0, 19.2);
}

void ir_isr() {
  bool changeDir = digitalRead(IR0_IN);  //direction of interrupt change

  if(changeDir == 0) {  //falling edge interrupt, IR receiver turning on
    int i = micros();  //capture current time
    int tempInterval;  
    tempInterval = i - rpmPreviousMicros;  //subtract last time from current to get length of pulse
    if(tempInterval > 24000) {  //2500 rpm cap, ignore faster
      rpmInterval = tempInterval;
      rpmPreviousMicros = i; //save this time for next time
    }
    irLedFlag = true;  //ir led on
    //led_manager();     //call led manager to set led colors
    //FastLED.show();    //update leds
  }
  if(changeDir == 1) {  //rising edge interrupt, IR receiver turning off
    irLedFlag = false;  //ir led off
    //led_manager();     //call led manager to set led colors
    //FastLED.show();    //update leds
  }
}
