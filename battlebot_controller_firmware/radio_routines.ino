/*
<0,0,0,0,0,0,0,0,0,0,0>

<1,256,100,200,256,0,0,0,0,0,0>

<1,256,100,200,256,0,0,0,0,1,1234>

<1,256,100,200,256,0,0,0,0,2,55.37>

<1,256,100,200,256,0,0,0,0,3,TEST>
 */



void transmitroutine() {
  static char sendbuffer[60];
  static int sendlength = 0;
  static char itembuffer[20];
  //itoa(yawVar, sendbuffer, 10);
  //sprintf(sendbuffer, "A%dB%d", throttleVar, yawVar);
  dtostrf(armed, 1, 0, sendbuffer);  //add first variable to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(throttleVar, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(yawVar, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(moveVar, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(strafeVar, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(L1Var, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(R1Var, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(L2Var, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(R2Var, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer
  dtostrf(cmdValue, 1, 0, itembuffer);  //copy item to item buffer
  strcat(sendbuffer, itembuffer);  //add to sendbuffer
  strcat(sendbuffer, ",");  //add comma to sendbuffer

  if(cmdValue != 0) {
    switch(cmdVarType) {
      case 0:  //do nothing
        break;
      case 1:  //send an int
        dtostrf(sentVarInt, 1, 0, itembuffer);  //copy item to item buffer
        strcat(sendbuffer, itembuffer);  //add to sendbuffer
        strcat(sendbuffer, ",");  //add comma to sendbuffer
        break;
      case 2:  //send a float
        dtostrf(sentVarFloat, 1, 2, itembuffer);  //copy item to item buffer
        strcat(sendbuffer, itembuffer);  //add to sendbuffer
        strcat(sendbuffer, ",");  //add comma to sendbuffer
        break;
      case 3:  //send a char array string
        strcpy(itembuffer, sentVarString);
        strcat(sendbuffer, itembuffer);  //add to sendbuffer
        strcat(sendbuffer, ",");  //add comma to sendbuffer
        break;
      default: //error case
        Serial.println("CMD VARIABLE TYPE ERROR: ");
        break;
    }
  }
  //Serial.print("SENDBUFFER: ");
  //Serial.print(sendbuffer);
  //Serial.println("");
  sendlength = strlen(sendbuffer);
  
  Serial.print("sending to node ");
  Serial.print(TONODEID, DEC);
  Serial.print(", message [");
  for (byte i = 0; i < sendlength; i++)
    Serial.print(sendbuffer[i]);
  Serial.println("]");
  
  radio.send(TONODEID, sendbuffer, sendlength);
  
  sendlength = 0; // reset the packet
  
  sendlength = 0; // reset the packet
  cmdValue = 0;  //reset command value
  cmdVarType = 0;  //reset variable type
}

void receiveroutine() {
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[62];
  static int sendlength = 0;

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:
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
    
    radio.receiveDone(); //test to put radio back in receive mode maybe?
  }  
}

void parse_data() {      // split the data into its parts

    cmdValueReturn = 0;  //reset cmd value to neutral
    
    Serial.print("PARSING DATA: ");
    Serial.println(tempChars);
    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    robotArmed = atoi(strtokIndx);     // convert this part to an integer
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotXaccel = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotYaccel = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotZaccel = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotRPM = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotCalcRPM = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    robotTrimRPM = atoi(strtokIndx);     // convert this part to an integer
    /*
    switch(cmdValueReturn) {  //process command
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
    */
    
    //strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    //cmdValue = atoi(strtokIndx);     // convert this part to an integer
    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float
    //strtokIndx = strtok(NULL, ",");
    //strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
}

void show_parsed_data() {
    
}

void receiveroutineold() {  //receive telemetry from robot
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);
  }
}


void testcommunication() {
   // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[62];
  static int sendlength = 0;

  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).

  // If there is any serial input, add it to the buffer:

  if (Serial.available() > 0)
  {
    char input = Serial.read();

    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    // If the input is a carriage return, or the buffer is full:

    if ((input == '\r') || (sendlength == 61)) // CR or buffer full
    {
      // Send the packet!


      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(", message [");
      for (byte i = 0; i < sendlength; i++)
        Serial.print(sendbuffer[i]);
      Serial.println("]");

      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():

      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      }

      // If you don't need acknowledgements, just use send():

      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }

      sendlength = 0; // reset the packet
      //Blink(LED_BUILTIN,10);
    }
  }
  
  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:
  
  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    //Blink(LED_BUILTIN,10);
  }
}
