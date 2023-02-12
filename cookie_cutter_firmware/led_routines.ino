void led_set0 (int r, int g, int b) {
  for(int i = 0; i < NUMLEDS; i++) {
    LEDS_0[i] = CRGB(r,g,b);
  }
}

void led_set1 (int r, int g, int b) {
   for(int i = 0; i < NUMLEDS; i++) {
    LEDS_1[i] = CRGB(r,g,b);
  }
}

void led_clear0 () {
  for(int i = 0; i < NUMLEDS; i++) {
    LEDS_0[i] = CRGB::Black;
  }
}

void led_clear1 () {
  for(int i = 0; i < NUMLEDS; i++) {
    LEDS_1[i] = CRGB::Black;
  }
}

void led_manager () {  //quickly mixes colors together for multiple things touching LEDs
  int r; int g; int b;  //rgb variables
  if(aimLedFlag == true) {  //aim led flag
    b = 255;  //give it BLUE
  }
  if(irLedFlag == true) {  //aim led flag
    r = 255;  //give it RED
  }
  if(robotOrientation == 0) {
    led_set0(r,g,b);
    
  }
  if(robotOrientation == 1) {
    led_set1(r,g,b);
  }
}
