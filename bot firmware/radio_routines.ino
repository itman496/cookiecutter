

void radio_routine() {
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[62];
  static int sendlength = 0;

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:
    Serial.println("");
    Serial.println("");
    Serial.println("///////////////");
    Serial.print("START: ");
    Serial.print(millis());
    Serial.println("ms");
    
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++) {
      Serial.print((char)radio.DATA[i]);
      tempChars[i] = radio.DATA[i];  //store radio packet into the temp char array to be parsed
      //Serial.print(",");
    }
    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    parse_data();
    show_parsed_data();
    
    //Blink(LED_BUILTIN,10);

    //Serial.print(millis());
    //Serial.println("ms");
    
    //Serial.println("DELAYING 10ms");
    //delay(10);

    Serial.print(millis());
    Serial.println("ms");
    
    transmit_routine();

    Serial.print(millis());
    Serial.println("ms");
    radio.receiveDone(); //test to put radio back in receive mode maybe?
  }  
}

void parse_data() {      // split the data into its parts

    cmdValue = 0;  //reset cmd value to neutral
    
    Serial.print("PARSING DATA: ");
    Serial.println(tempChars);
    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    //strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
    armed = atoi(strtokIndx);     // convert this part to an integer
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    throttleCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    yawCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    moveCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    strafeCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    flipRobot = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    blitzModeCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    leftTrimCmd = atoi(strtokIndx);     // convert this part to an integer
    
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    rightTrimCmd = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    cmdValue = atoi(strtokIndx);     // convert this part to an integer

    switch(cmdValue) {  //process command
      case 0:  //do nothing
        break;

      case 1:  //receive an int
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        sentVarInt = atoi(strtokIndx);     // convert this part to an integer
        break;

      case 2:  //receive a float
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        sentVarFloat = atof(strtokIndx);     // convert this part to an integer
        break;
        
      case 3:  //receive a string
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        strcpy(sentVarString, strtokIndx); // copy it to messageFromPC
        break;
      
      default:  //invalid
        Serial.println("INVALID COMMAND");
        break;
    }

    int TRM = (leftTrimCmd * -1) + rightTrimCmd;  //combine trim commands so if both press, cancels out
    if(trimLock == false && TRM == 1) {  //is lock false, and trim positive?
      rpmTrim++;  //increase trim
      trimLock = true;  //lock so it doesn't spam
    }
    if(trimLock == false && TRM == -1) {
      rpmTrim--;  //decrease trim
      trimLock = true;  //lock so it doesn't spam
    }
    if(TRM == 0) {  //are both buttons released?
      trimLock = false;  //unlock trim commands again
    }
    
    headingCmd = headingcalc(strafeCmd, moveCmd);  //calculate what direction is requested, and how much
    intensityCmd = intensitycalc(strafeCmd, moveCmd);
    
    //strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    //cmdValue = atoi(strtokIndx);     // convert this part to an integer
    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float
    //strtokIndx = strtok(NULL, ",");
    //strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
}

void show_parsed_data() {
    Serial.println("");
    Serial.print("ARMED: ");
    Serial.print(armed);
    Serial.print(" THR: ");
    Serial.print(throttleCmd);
    Serial.print(" YAW: ");
    Serial.print(yawCmd);
    Serial.print(" MOVE: ");
    Serial.print(moveCmd);
    Serial.print(" STRAFE: ");
    Serial.print(strafeCmd);
    Serial.print(" L-TRIM: ");
    Serial.print(leftTrimCmd);
    Serial.print(" R-TRIM: ");
    Serial.print(rightTrimCmd);
    Serial.print(" BLITZ: ");
    Serial.print(blitzModeCmd);
    Serial.print(" FLIPPED: ");
    Serial.print(flipRobot);
    Serial.println("");

    Serial.print("RECEIVED COMMAND: ");
    Serial.println(cmdValue);
    
    Serial.print("LAST RECEIVED INT: ");
    Serial.println(sentVarInt);
    Serial.print("LAST RECEIVED FLOAT: ");
    Serial.println(sentVarFloat);
    Serial.print("LAST RECEIVED STRING: ");
    Serial.println(sentVarString);
}

void transmit_routine() {
  static char sendbuffer[60];
  static int sendlength = 0;
  static char itembuffer[20];
  //itoa(yawVar, sendbuffer, 10);
  //sprintf(sendbuffer, "A%dB%d", throttleVar, yawVar);
  dtostrf(armed, 1, 0, sendbuffer);  //add first variable to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(filteredRPM, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(headingCmd, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(intensityCmd, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(rpmAvg.getAvg(), 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(calculatedRPM, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  sendlength = strlen(sendbuffer);
  dtostrf(rpmTrim, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  sendlength = strlen(sendbuffer);
  
  Serial.print("sending to node ");
  Serial.print(TONODEID, DEC);
  Serial.print(", message [");
  for (byte i = 0; i < sendlength; i++)
    Serial.print(sendbuffer[i]);
  Serial.println("]");

  radio.send(TONODEID, sendbuffer, sendlength);

  sendlength = 0; // reset the packet
}
