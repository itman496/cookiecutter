void touchroutine() {
  touching = false;   //unset global one shot touch variable
  bool isTouched = tft.touched();  //sense if touched
  if (isTouched == true) {         //if touched
    touchLockMillis = millis();    //set last millis since touch 
  }
  if (isTouched == true && touchLock == false) {  //if touched, and if not locked
    touchLock = true;                             //set touch lock

    for (int x = 0; x < tSamples; x++) {  //iterate sample reads or whatever
      tft.touchReadPixel(&tx, &ty);       //read directly in pixel
      tx = (tx * -1) + 800;               //mirror X and Y because its stupid
      ty = (ty * -1) + 480;
      txArray[x] = tx;  //store variable X in array
      tyArray[x] = ty;  //store variable Y in array
      delay(tDelay);    //wait one millisecond to space out readings
    }
    int tempX = 0;                        //temp x location, needs to start at zero for some reason
    for (int x = 0; x < tSamples; x++) {  //add up array
      //Serial.print(txArray[x]);  Serial.print(" ");
      tempX += txArray[x];
    }
    touchX = tempX / tSamples;  //averages array

    int tempY = 0;                        //temp y location
    for (int x = 0; x < tSamples; x++) {  //add up array
      tempY += tyArray[x];
    }
    touchY = tempY / tSamples;                   //averages array
    touching = true;  //set global one shot touch variable
    //tft.fillCircle(touchX, touchY, 50, 0xFFFF);  //draws circle
    
  }

  int timeSinceLastTouch = millis() - touchLockMillis;  //calculate time since last touched
  if (timeSinceLastTouch > touchLockDelay) {            //if time since last touch is greater then set delay
    touchLock = false;                                  //unlock touch
  }
  isTouched = false;  //set back to not touched anymore
}
