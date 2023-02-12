void robotselectscreen() {
  tft.fillRect(0,0,800,380,RA8875_BLUE);  //create border color
  tft.fillRect(4,24,792,352, RA8875_BLACK);  //create black background
  drawbatterymeter(730,4,RA8875_WHITE);  //draw battery meter
  
  tft.setCursor(30,40);
  tft.setFontScale(3);
  tft.setTextColor(RA8875_WHITE);
  tft.println("MODEL SELECT!");
  tft.setFontScale(1.5);
  tft.setCursor(20,100);
  tft.println("...Is what this would say if you owned more then one robot!  Make more!");

  drawbottombutton(0,380,"ROBOT","STATUS",false,101,RA8875_WHITE);
  drawbottombutton(206,380,"RADIO","SETTINGS",false,201,RA8875_RED, RA8875_WHITE);
  drawbottombutton(414,380,"ROBOT","SETTINGS",false,301,RA8875_GREEN);
  drawbottombutton(620,380,"ROBOT","SELECT",true,401,RA8875_BLUE, RA8875_WHITE);
}

void radiosettingsscreen() {
  float testVar = ((sin(float(millis())/500))*512)+512;
  
  tft.fillRect(0,0,800,380,RA8875_RED);  //create border color
  tft.fillRect(4,24,792,352, RA8875_BLACK);  //create black background
  drawbatterymeter(730,4,RA8875_WHITE);  //draw battery meter
  
  drawshoulderbutton(30,60,"LEFT TRIM","L2",L2Var);  //draw shoulder button indicators
  drawshoulderbutton(30,190,"FLIP (VIBR)","L1",L1Var);  //draw shoulder button indicators
  drawshoulderbutton(620,60,"RIGHT TRIM","R2",R2Var);  //draw shoulder button indicators
  drawshoulderbutton(620,190,"BLITZ (IR)","R1",R1Var);  //draw shoulder button indicators
  drawjoystickview(210,60,yawVar,throttleVar-255,512,"LEFT","YAW","THRT");  //make joystick preview
  drawjoystickview(410,60,strafeVar,moveVar,512,"RIGHT","STRF","MOVE");  //make joystick preview

  drawbottombutton(0,380,"ROBOT","STATUS",false,101,RA8875_WHITE);
  drawbottombutton(206,380,"RADIO","SETTINGS",true,201,RA8875_RED, RA8875_WHITE);
  drawbottombutton(414,380,"LINK","SETTINGS",false,202,RA8875_RED, RA8875_WHITE);
  drawbottombutton(620,380,"CONTROL","TEST",false,203,RA8875_RED, RA8875_WHITE);
}

void robotstatusscreen() {
  tft.fillRect(0,0,800,380,RA8875_WHITE);  //create border color
  tft.fillRect(4,24,792,352, RA8875_BLACK);  //create black background
  drawbatterymeter(730,4,RA8875_BLACK);  //draw battery meter

  tft.setCursor(40,30);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE);
  tft.println("ACCELEROMETER DATA");
  tft.print(" X: ");  tft.println(robotXaccel);
  tft.print(" Y: ");  tft.println(robotYaccel);
  tft.print(" Z: ");  tft.println(robotZaccel);
  tft.print(" RPM: ");  tft.println(robotRPM);
  tft.print(" cRPM: ");  tft.println(robotCalcRPM);

  tft.setCursor(320,75);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE);
  tft.print("RPM TRIM: ");  tft.println(robotTrimRPM);
  

  drawbottombutton(0,380,"ROBOT","STATUS",true,101,RA8875_WHITE);
  drawbottombutton(206,380,"RADIO","SETTINGS",false,201,RA8875_RED, RA8875_WHITE);
  drawbottombutton(414,380,"ROBOT","SETTINGS",false,301,RA8875_GREEN);
  drawbottombutton(620,380,"ROBOT","SELECT",false,401,RA8875_BLUE, RA8875_WHITE);
}

void robotsettingsscreen() {
  tft.fillRect(0,0,800,380,RA8875_GREEN);  //create border color
  tft.fillRect(4,24,792,352, RA8875_BLACK);  //create black background
  drawbatterymeter(730,4,RA8875_BLACK);  //draw battery meter

  drawbutton(200,100,"INT +10",false,901);
  drawbutton(200,210,"INT -100",false,902);
  
  tft.setCursor(20,40);
  tft.setFontScale(3);
  tft.setTextColor(RA8875_WHITE);
  tft.print("ROBOT SETTINGS!");

  drawbottombutton(0,380,"ROBOT","STATUS",false,101,RA8875_WHITE);
  drawbottombutton(206,380,"RADIO","SETTINGS",false,201,RA8875_RED, RA8875_WHITE);
  drawbottombutton(414,380,"ROBOT","SETTINGS",true,301,RA8875_GREEN);
  drawbottombutton(620,380,"ROBOT","SELECT",false,401,RA8875_BLUE, RA8875_WHITE);
}

void testscreen() {
  drawbatterymeter(730,4,RA8875_WHITE);  //draw battery meter
  
  drawbutton(400,100,"TEST",false,69);
  drawbutton(400,210,"TEST2",false,420);

  drawbottombutton(0,380,"ROBOT","STATUS",false,101,RA8875_WHITE);
  drawbottombutton(206,380,"RADIO","SETTINGS",false,201,RA8875_RED, RA8875_WHITE);
  drawbottombutton(414,380,"ROBOT","SETTINGS",false,301,RA8875_GREEN);
  drawbottombutton(620,380,"ROBOT","SELECT",false,401,RA8875_BLUE, RA8875_WHITE);
  
  float testVar = ((sin(float(millis())/500))*100)+100;
  tft.setCursor(20,40);
  tft.setFontScale(3);
  tft.setTextColor(RA8875_WHITE);
  tft.print("TEST!!");
  tft.print(testVar);
  tft.print("BTN: ");
  tft.print(buttonPress);
  tft.fillRect(50,150,testVar,25, RA8875_MAGENTA);

  tft.ringMeter(
    testVar,
    0,
    200,
    50,
    180,
    100,
    "mA",
    7,
    0x2945,
    120,
    5); // Draw analogue meter
  
}
