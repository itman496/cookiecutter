void drawbatterymeter (int x, int y, int color) {
  tft.drawRect(x,y,60,15,color);  //draw outer battery thing
  tft.fillRect(x+60,y+3,4,9,color);  //draw nub
  int battBar = map(packPercent,0,100,0,59);  //map battery percentage to fill bar
  tft.fillRect(x+1,y+1,battBar,14,color);  //fill in bar
  tft.setTextColor(color);  //set text color and size up
  tft.setFontScale(.85);
  tft.setCursor(x-27,y+7,true);
  tft.print(packPercent,0); //draw percentage, no decimal points
  tft.print("%");
}

void drawshoulderbutton (int x, int y, String label, String intLabel, bool activated) {
  tft.fillRoundRect(x,y,150,80,16,RA8875_WHITE);  //draw shoulder button frames
  if(activated == true) {
    tft.fillRoundRect(x+4,y+4,142,72,12,RA8875_RED);
    tft.setTextColor(RA8875_WHITE,RA8875_RED);
  }
  else {
    tft.fillRoundRect(x+4,y+4,142,72,12,RA8875_BLACK);
    tft.setTextColor(RA8875_WHITE,RA8875_BLACK);
  }
  tft.drawLine(x+4,y+40,x+146,y+40,0x8410);  //draw crosshair things
  tft.setFontScale(2);
  tft.setCursor(x+75,y+40,true);
  tft.print(intLabel);
  tft.setTextColor(RA8875_RED);
  tft.setFontScale(1);
  tft.setCursor(x+75,y-15,true);
  tft.print(label);
}


void drawjoystickview (int x, int y, int xAxis, int yAxis, int scale, String label, String xLabel, String yLabel) {
  tft.fillRoundRect(x,y,180,180,20,RA8875_WHITE);  //draw outside border
  tft.fillRoundRect(x+4,y+4,172,172,16,RA8875_BLACK);  //fill in center black
  tft.drawLine(x+90,y+4,x+90,y+176,0x8410);  //draw crosshairs
  tft.drawLine(x+4,y+90,x+176,y+90,0x8410);
  
  //tft.drawRect(x,y+184,180,12,RA8875_GREEN);
  int xAxist = map(xAxis,(scale/2)*-1,scale/2,15,165);  //map joystick inputs to fit widgit
  int yAxist = map(yAxis,(scale/2)*-1,scale/2,165,15);  //map joystick inputs to fit widgit
  tft.fillCircle(x+xAxist,y+yAxist,15,0xA800);  //draw joystick circle
  tft.fillCircle(x+xAxist,y+yAxist,10,RA8875_RED);
  xAxis = map(xAxis,(scale/2)*-1,scale/2,-100,100);  //map input to percentages
  yAxis = map(yAxis,(scale/2)*-1,scale/2,-100,100);
  tft.setCursor(x+2,y+182);  //print percent data
  tft.setFontScale(1);
  tft.setTextColor(RA8875_RED);
  tft.print(xLabel); tft.print(": ");  
  tft.setTextColor(RA8875_WHITE);
  tft.print(xAxis);  tft.print("%");
  tft.setCursor(x+2,y+210);
  tft.setTextColor(RA8875_RED);
  tft.print(yLabel); tft.print(": ");  
  tft.setTextColor(RA8875_WHITE);
  tft.print(yAxis);  tft.print("%");
  tft.setTextColor(RA8875_RED);
  tft.setCursor(x+90,y-15,true);  //print title
  tft.print(label);
}


void drawbutton (int x, int y, String label, bool activated, int functionNum) { 
  bool pressed = false;
  if(touchX >= x && touchX <= x+150 && touchY >= y && touchY <= y+100 && touching == true) {
    activated = true;
    pressed = true;
  }
  tft.setCursor(x+75,y+50,true);
  tft.setFontScale(1.5);
  if(activated == false) {
    tft.drawRoundRect(x,y,150,100,16,RA8875_WHITE);
    tft.setTextColor(RA8875_WHITE);
  }
  else {
    tft.fillRoundRect(x,y,150,100,16,RA8875_WHITE);
    tft.setTextColor(RA8875_BLACK);
  }

  tft.print(label);
  if(pressed) {
    Serial.println(functionNum);
    buttonrouter(functionNum);
  }
}


void drawbottombutton (int x, int y, String label, String label2, bool activated, int functionNum, int color, int textColor=RA8875_BLACK) { 
  bool pressed = false;
  if(touchX >= x && touchX <= x+180 && touchY >= y && touchY <= y+100 && touching == true) {
    activated = true;
    pressed = true;
  }
  tft.setFontScale(1.5);
  if(activated == false) {
    tft.drawRoundRect(x,y+5,180,95,16,color);
    tft.setTextColor(RA8875_WHITE);
  }
  else {
    tft.fillRoundRect(x,y,180,100,16,color);
    tft.fillRect(x,y,180,18,color);
    tft.setTextColor(textColor);
  }
  tft.setCursor(x+90,y+35,true);
  tft.print(label);
  tft.setCursor(x+90,y+65,true);
  tft.print(label2);
  if(pressed) {
    Serial.println(functionNum);
    buttonrouter(functionNum);
  }
}
