void calc_rpm() {
  currentRPM = 60000000 / rpmInterval;  //amount of us in a minute divided by us per pulse
  rpmAvg.reading(currentRPM);  //add rpm to average
}

void control_loop() {  //main control loop of the meltybrain
  accel_rpm();  //calculate rpm from accel readings
  if(calculatedRPM < 150) {  //if slow then don't bother
    calculatedRPM = 0;  //clamp rpm down to zero for low speed noise issues
    robotMode = 1;  //set robot to aquire index mode
  }
  
  if(calculatedRPM >= 150) {
    if(robotMode == 1) {  //indexing mode
      indexMicros = micros();  //capture current microsecond count
      calculate_timings();  //calculate future event timings      
      robotMode = 2;  //calculations done, set mode to 2 to run the cycle, and not center again
    }
    
    if(robotMode == 2) {  //cycle run mode
      accelInterval++;  //increment accelInterval
      if(accelInterval > 9) {  //once its bigger then 10 (ten loops through ISR)
        calculate_timings();  //recalculate future event timings every loop
        accelInterval = 0;  //reset accelInterval
      }
      if(micros() > futureIndex) {  //we have hit the next index, set mode back to 1 to redo cycle!
        robotMode = 1;
      }
      //*************AIMING LED SECTION****************
      if(aimWaypoint == 0) {
        if(micros() >= aimLedEndTime) {  //is it time to turn off led?
          aimLedFlag = false;  //turn off aim leds
          //led_manager();       //update led color mixing
          led_set0(0,0,0);
          FastLED.show();      //update leds
          aimWaypoint = 1;  //advance to next waypoint
        }
      }
      if(aimWaypoint == 1) {
        if(micros() >= aimLedStartTime) {  //is it time to turn off led?
          aimLedFlag = true;  //turn on aim leds
          //led_manager();      //update led color mixing
          led_set0(0,255,0);
          FastLED.show();     //update leds
          aimWaypoint = 0;  //reset to starting waypoint
        }
      }
      //************MOTOR DRIVE SECTION****************
      if(driveWaypoint == 1) {
        if(micros() >= throttleUpTime0) {  //if its time to throttle motor up
          digitalWrite(ESC0_OUT, HIGH);  //show motor power on debug led

          throttleDownTime0 = throttleUpTime0 + (driveDuration * degreeSlice);  //calculate time to turn off motor
          throttleUpTime1 = throttleUpTime0 + (180 * degreeSlice);
          driveWaypoint = 2;  //advance to next waypoint
        }
      }
      if(driveWaypoint == 2) {
        if(micros() >= throttleDownTime0) {  //if its time to throttle motor down
          digitalWrite(ESC0_OUT, LOW);  //show motor power on debug led

          driveWaypoint = 3;  //advance to next waypoint
        }
      }
      if(driveWaypoint == 3) {
        if(micros() >= throttleUpTime1) {  //if its time to throttle motor up
          digitalWrite(ESC1_OUT, HIGH);  //show motor power on debug led
          throttleDownTime1 = throttleUpTime1 + (driveDuration * degreeSlice);  //calculate time to turn off motor
          driveWaypoint = 0;  //reset to starting waypoint
        }
      }
      if(micros() >= throttleDownTime1) {  //if its time to throttle motor down
          digitalWrite(ESC1_OUT, LOW);  //show motor power on debug led
        }
    }  
  }
}

void calculate_timings() {  //calculates future timing for events based on RPM
  rotationPeriod = 60000000 / (calculatedRPM + rpmTrim + (yawCmd/7) + (deltaRPM * 40));  //how long will a full rotation at this rpm last?
  futureIndex = indexMicros + rotationPeriod;  //predict when the next index time will be
  int intervalSlice = rotationPeriod / 12;

  aimLedEndTime = indexMicros + intervalSlice;  //shut LED off after it was on from previous cycle at 11'oclock
  aimLedStartTime = futureIndex - intervalSlice;  //turn on LED at 1'oclock

  degreeSlice = rotationPeriod / 360;  //slice rotation period into 1 degree chunks
  if(intensityCmd > 5  && driveWaypoint == 0) {  //if movement is detected
    throttleUpTime0 = ((headingCmd + driveOffset) * degreeSlice) + indexMicros;  //what direction am I going + offset degrees
    driveWaypoint = 1;  //move onto next waypoint
  }
    
}

void accel_rpm() {
  //=SQRT(RCF(G)/((Rcm/10)*1.118))*1*100  
  float divider = 32767 / ACCEL_RANGE; //generate divider from accel scale
  //float gForce = xAvg.getAvg() / divider; //convert to gforce from raw value
  float gForce = filteredRPM / divider; //convert to gforce from raw value (LOW PASS)
  deltaInterval++;  //increment delta interval
  if(deltaInterval > 50) {  //every 50 loops
    deltaInterval = 0;  //reset interval
    deltaRPM = calculatedRPM - previousRPM;  //calculate delta from old number which is far away in time
    deltaAvg.reading(deltaRPM);  //average readings
    previousRPM = calculatedRPM;  //save rpm reading for next time
  }
  
  calculatedRPM = sqrt(gForce / (CHIP_OFFSET * 1.118)) * 100;  //calculate rpm
  
}

void blinkercallback() {
  fastBlink = !fastBlink;  //flip fastblink
  if(fastBlink = true) {
    slowBlink = !slowBlink;  //every other time, flip slow blink
  }
}

int headingcalc(int joyX, int joyY) {  //function to calculate heading from joystick inputs
  double xTrig = mapfloat(joyX, -joyScale, joyScale, -1, 1);  //turn raw joystick inputs into something trig friendly
  double yTrig = mapfloat(joyY, -joyScale, joyScale, -1, 1);
  
  double head = atan2(yTrig, xTrig);  //calculate heading in radians
  int headingDeg = ((head*4068) / 71) + 180 + 90; //convert to degrees, offset 270
  if(headingDeg > 359) {  //if about to wrap around
    headingDeg = headingDeg - 360;  //push the other direction
  }

  return headingDeg;
}

int intensitycalc(int joyX, int joyY) {  //function to calculate amount joysticks have been moved
  int x = abs(joyX);  //make positive or negative always positive
  int y = abs(joyY);

  int moveAmt = x;  //set to x by default
  if(y > x) {  //if y is bigger then x, use y instead
    moveAmt = y;
  }
  return moveAmt;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
